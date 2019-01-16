#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include"amr.h"

extern int length_amr;

double get_bitrate(int row, int ft) {                     //获得比特率
    double bitrate_table[2][9] = {
        {4.75,5.15,5.90,6.70,7.40,7.95,10.20,12.20,1.80},
        {6.60,8.85,12.65,14.25,15.85,18.25,19.85,23.05,23.85}
    };
    return bitrate_table[row - 1][ft];
}

int  get_frame_size(int row, int ft) {                    //获得帧字节大小
    int frame_size_table[2][9] = {
        {13,14,16,18,20,21,27,32,6},
        {18,24,33,37,41,47,51,59,61}
    };
    return frame_size_table[row - 1][ft];
}

int get_head_size(FILE *fp){                              //获得文件头字节大小
    fread(file_head, 19, 1, fp);
    int i;
    for (i = 0;i < 15;i++) {                               //判断格式类型
        if (file_head[i] == '\n') {
            i++;
            break;
        }
    }
    if (i == 6) {
        if(!strncasecmp(file_head,"#!AMR\n",6) || !strncasecmp(file_head,"#!amr\n",6)){
            printf("This is AMR-NB Mono\n");
        }
        else{
            printf("The word of header error");
            exit(-1);
        }
    }
    else if (i == 9) {
        if(!strncasecmp(file_head,"#!AMR-WB\n",6) || !strncasecmp(file_head,"#!amr-wb\n",6)){
            printf("This is AMR-WB Mono\n");
        }
        else{
            printf("The word of header error");
            exit(-1);
        }
    }
    else if (i == 12) {
        channel_number = file_head[15] & 0xF;
        if(!strncasecmp(file_head,"#!AMR_MC1.0\n",6) || !strncasecmp(file_head,"#!amr_mc1.0\n",6)){
            printf("This is AMR-NB with %d channels", channel_number);
            i = i + 4;
        }
        else{
            printf("The word of header error");
            exit(-1);
        }
    }
    else if (i == 15) {
        channel_number = file_head[18] & 0xF;
        if(!strncasecmp(file_head,"#!AMR-WB_MC1.0\n",6) || !strncasecmp(file_head,"#!amr-wb_mc1.0\n",6)){
            printf("This is AMR-WB with %d channels", channel_number);
            i = i + 4;
        }
        else{
            printf("The word of header error");
            exit(-1);
        }
    }
    else {
        printf("ERROR:This is not a complete arm file");
        exit(-1);
    }
    return i;
}


void amr(FILE *fp) {
    char   re_frame_head[1];                            //待检测帧好坏的帧头
    double  bitrate;                                    //比特率
    char   frame_head[1];                               //帧头（检测帧好坏）
    char   frames[1024];                                //帧
    int frame_size;                                     //帧字节大小
    int head_size;                                      //文件头所占字节大小
    int complete_frame_number;                          //好帧的数量
    complete_frame_number = 0;
    int row;                                            //多维数组的行
    int ft;                                             //FT值，多维数组的列-1
    int q;                                              //Q值
    int length;                                         //长度（多少秒）
    int p;                                              //帧头第一位
    int pp;                                             //帧头最后两位
    int i = 1;
    int j = 1;
    int k = 1;
    int m = 1;

    head_size = get_head_size(fp);
    printf("FILE_HEAD_SIZE:\t%d bytes\n", head_size);
    fseek(fp, head_size, SEEK_SET);                                         //跳过文件头

FIND_COMPLETE_FRAME:                                               //找好帧
    fread(frames, 1, sizeof(frame), fp);
    frame_head[0] = frames[0];
    q = (frame_head[0] & 0x4) >> 2;
    p = (frame_head[0] & 0x80) >> 7;
    pp = (frame_head[0] & 0x3);
    ft = (frame_head[0]) >> 3;
    if ((head_size == 6 || head_size == 16) && q == 1 && p == 0 && pp == 0 && \
            (ft==0 || ft==1 || ft==2 || ft==3 || ft==4 || ft==5 || ft ==6 || ft==7)) {                  //AMR-NB好帧
        row = 1;
        bitrate = get_bitrate(row, ft);
        printf("Bitrate:\t%.2f kbit/s\n", bitrate);
        frame_size = get_frame_size(row, ft);
        printf("FrameSize:\t%d bytes\n", frame_size);
        complete_frame_number++;
    }
    else if ((head_size == 9 || head_size == 19) && q == 1 && p == 0 && pp == 0 && \
            (ft==0 || ft==1 || ft==2 || ft==3 || ft==4 || ft==5 || ft ==6 || ft==7 || ft==8)) {         //AMR-WB好帧
        row = 2;
        bitrate = get_bitrate(row, ft);
        printf("Bitrate:\t%.2f kbit/s\n", bitrate);
        frame_size = get_frame_size(row, ft);
        printf("FrameSize:\t%d bytes\n", frame_size);
        complete_frame_number++;                                   //好帧数量+1
    }
    else if((head_size == 6 || head_size == 16) && q == 1 && p == 0 \
            && pp == 0 && (ft==8 || ft==9 || ft==10 || ft==11 )){                   //SID
        while(k){
            printf("There is a SID Frame\n");
            k--;
        }
        goto FIND_COMPLETE_FRAME;                                    //读下一字节，找到好帧
    }
    else if((head_size == 9 || head_size == 19) && q == 1 && p == 0 \
            && pp == 0 && ft==9){                                                //Comfort Noise Frame
        while(m){
            printf("There is a Comfort Noise Frame\n");
            m--;
        }
        goto FIND_COMPLETE_FRAME;                                    //读下一字节，找到好帧
    }
    else {                                                           //坏帧
        while(i){
            printf("There is a wrong frame\n");
            i--;
        }
        goto FIND_COMPLETE_FRAME;                                    //读下一字节，找到好帧
    }

    while (!feof(fp)) {
        fseek(fp, frame_size - 1, SEEK_CUR);                               //到下一帧帧头处
RE_FIND_COMPLETE_FRAME:
        fread(frames, 1, sizeof(frame), fp);
        re_frame_head[0] = frames[0];
        ft = (re_frame_head[0]) >> 3;
        q = (re_frame_head[0] & 0x4) >> 2;
        p = (re_frame_head[0] & 0x80) >> 7;
        pp = (re_frame_head[0] & 0x3);
        if (re_frame_head[0] == frame_head[0]) {                         //好帧
            complete_frame_number++;                                      //好帧数量+1
        }
        else if((head_size == 6 || head_size == 16) && (ft==0 || ft==1 || ft==2 || ft==3 \
                    || ft==4 || ft==5 || ft ==6 || ft==7) && q==1 && p==0 && pp==0){        //amr-nb采样率模式改变
            printf("The mode of sampling rate has changed.\n");
            bitrate = get_bitrate(row, ft);
            printf("Bitrate:\t%.2f kbit/s\n", bitrate);
            frame_size = get_frame_size(row, ft);
            printf("FrameSize:\t%d bytes\n", frame_size);
            complete_frame_number++;                                   //好帧数量+1
            frame_head[0] = re_frame_head[0];
        }
        else if((head_size == 9 || head_size == 19) && (ft==0 || ft==1 || ft==2 || ft==3 \
                    || ft==4 || ft==5 || ft ==6 || ft==7 || ft ==8) && q==1 && p==0 && pp==0){        //amr-wb采样率模式改变
            printf("The mode of sampling rate has changed.\n");
            bitrate = get_bitrate(row, ft);
            printf("Bitrate:\t%.2f kbit/s\n", bitrate);
            frame_size = get_frame_size(row, ft);
            printf("FrameSize:\t%d bytes\n", frame_size);
            complete_frame_number++;                                   //好帧数量+1
            frame_head[0] = re_frame_head[0];
        }
        else if((head_size == 6 || head_size == 16) && q == 1 && p == 0 \
                && pp == 0 && (ft==8 || ft==9 || ft==10 || ft==11 )){                   //SID
            while(k){
                printf("There is a SID Frame\n");
                k--;
            }
            goto RE_FIND_COMPLETE_FRAME;                                    //读下一字节，找到好帧
        }
        else if((head_size == 9 || head_size == 19) && q == 1 && p == 0 \
                && pp == 0 && ft==9){                                                //Comfort Noise Frame
            while(m){
                printf("There is a Comfort Noise Frame\n");
                m--;
            }
            goto RE_FIND_COMPLETE_FRAME;                                    //读下一字节，找到好帧
        }
        else{                                                                 //坏帧
            while(j){
                printf("There is a bad frame or the mode of sampling rate error\n");
                j--;
            }
            goto RE_FIND_COMPLETE_FRAME;                             //读下一字节，找到好帧
        }
    }
    printf("CompleteFrameNumber:%d \n", complete_frame_number);

    length = complete_frame_number * 20 / 1000;
    printf("Length:\t\t%d s\n\n\n", length);
    length_amr += length;
}
