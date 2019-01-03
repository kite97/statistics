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

    head_size = get_head_size(fp);
    printf("FILE_HEAD_SIZE:\t%d bytes\n", head_size);
    fseek(fp, head_size, SEEK_SET);                                         //跳过文件头

FIND_COMPLETE_FRAME:                                               //找好帧
    fread(frames, 1, sizeof(frame), fp);
    frame_head[0] = frames[0];
    q = (frame_head[0] & 0x4) >> 2;
    ft = (frame_head[0]) >> 3;
    if (q == 1) {                                                     //好帧
        if (head_size == 6 || head_size == 16) {                                      //AMR-NB
            row = 1;
            bitrate = get_bitrate(row, ft);
            printf("Bitrate:\t%.2f kbit/s\n", bitrate);
            frame_size = get_frame_size(row, ft);
            printf("FrameSize:\t%d bytes\n", frame_size);
            complete_frame_number++;
        }
        else {                                                        //AMR-WB
            row = 2;
            bitrate = get_bitrate(row, ft);
            printf("Bitrate:\t%.2f kbit/s\n", bitrate);
            frame_size = get_frame_size(row, ft);
            printf("FrameSize:\t%d bytes\n", frame_size);
            complete_frame_number++;                                   //好帧数量+1
        }
    }
    else {                                                           //坏帧
        goto FIND_COMPLETE_FRAME;                                    //读下一字节，找到好帧
    }

    while (!feof(fp)) {
        fseek(fp, frame_size - 1, SEEK_CUR);                               //到下一帧帧头处
RE_FIND_COMPLETE_FRAME:
        fread(frames, 1, sizeof(frame), fp);
        re_frame_head[0] = frames[0];
        if (re_frame_head[0] == frame_head[0]) {                         //好帧
            complete_frame_number++;                                      //好帧数量+1
        }
        else {                                                           //坏帧
            goto RE_FIND_COMPLETE_FRAME;                             //读下一字节，找到好帧
        }
    }
    printf("CompleteFrameNumber:%d \n", complete_frame_number);

	length = complete_frame_number * 20 / 1000;
	printf("Length:\t\t%d s\n\n\n", length);
        length_amr += length;
}
