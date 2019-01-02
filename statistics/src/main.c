#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "wave.h"

#define WAVE 1
#define MP3 2
#define AMR 3
#define OTHER 4

extern void wave(wave_t* wav, FILE *fp);
extern void mp3(FILE *fp);
extern void amr(FILE *fp);

int length_wave = 0;
int length_mp3 = 0;
int length_amr = 0;
int length_all = 0;
int num_soft_link = 0;
char address[512];

//判断文件类型
int type_check(wave_t *wav){
    if (!strncasecmp(wav->riff.wave_id, "wave", 4)){
        return WAVE;
    }
    else if (!strncasecmp((char *)wav, "ID3", 4) || !strncasecmp((char *)wav, "id3", 3 )){
        return MP3;
    }
    else if (!strncasecmp((char *)wav, "#!AMR", 5)){
        return AMR;
    }
    else{
        return OTHER;
    }
}

//返回文件夹中的一个文件名（此文件名未返回过）
char*  do_ls(char dirname[], int* type){
    char* get_name;
    static char have_name[512];
    static char have_dir[512];
    static char have_soft_link[512];
    DIR *dir_ptr;
    struct dirent *direntp;
    dir_ptr = opendir(dirname);
    if(dir_ptr == NULL){
        printf("Error: can not open %s\n",dirname);
    }
    else{
        while(direntp = readdir(dir_ptr)){
            if(strcmp(direntp->d_name, "..") == 0 || strcmp(direntp->d_name,".") == 0){
                continue;
            }
            else if(direntp->d_type == 4){
                *type = 4;
                if(strstr(have_dir,direntp->d_name)){
                    continue;
                }
                else{
                    strcat(have_dir, direntp->d_name);
                    get_name = direntp->d_name;
                    closedir(dir_ptr);
                    return get_name;
                }
            }
            else if(direntp->d_type == 8){
                *type = 8;
                if(strstr(have_name,direntp->d_name)){
                    continue;
                }
                else{
                    strcat(have_name, direntp->d_name);
                    get_name = direntp->d_name;
                    closedir(dir_ptr);
                    return get_name;
                }
            }
            else if(direntp->d_type == 10){
                *type = 10;
                if(strstr(have_soft_link,direntp->d_name)){
                    continue;
                }
                else{
                    strcat(have_soft_link, direntp->d_name);
                    num_soft_link++;
                    get_name = direntp->d_name;
                    closedir(dir_ptr);
                    return get_name;
                }
            }
        }
    }
}

//遍历目录中的音频文件并分析
void ergodic_statistics(char* dirname){
    static char buff[1024];
    char* file_name;
    char temp_address[512];
    char new_dirname[512];
    int* type = (int*)malloc(sizeof(int));
    sprintf(address,"%s",dirname);
    while(file_name = do_ls(dirname ,type)){
        if(*type == 4){
            sprintf(new_dirname,"%s/%s",dirname,file_name);
            ergodic_statistics(new_dirname);
        }
        else if(*type == 10){
            sprintf(temp_address,"%s/%s",address,file_name);
            printf("The address of soft-link %d is %s\n\n",num_soft_link,temp_address);
        }
        else if(*type == 8){
            printf("%s:\n",file_name);
            char address[64];
            sprintf(address,"%s/%s",dirname,file_name);
            FILE *fp = fopen(address, "rb");
            if(!fp){
                printf("can't open audio file\n");
                exit(-1);
            }
            fread(buff, 1, sizeof(buff) , fp);
            wave_t *wav = (wave_t *)&buff[0];
            fseek(fp, 0 , SEEK_SET);
            int i = type_check(wav);
            switch(i)
            {
                case WAVE:
                    wave(wav,fp);
                    break;
                case MP3:
                    mp3(fp);
                    break;
                case AMR:
                    amr(fp);
                    break;
                case OTHER:
                    printf("This is an audio file of other types\n");
                    break;
            }
        }
    }
}

//主程序
int main(int argc, char* argv[]) {
    char* dirname;
    if(argc == 1){
        printf("There is no file\n");
    }
    else{
        dirname = argv[1];
        ergodic_statistics(dirname);
        printf("Having %d soft-link\n",num_soft_link);
        length_all = length_wave + length_mp3 +length_amr;
        printf("The length of wave is:\t%d s\n",length_wave);
        printf("The length of mp3 is:\t%d s\n",length_mp3);
        printf("The length of amr is:\t%d s\n",length_amr);
        printf("The length of all is:\t%d s\n",length_all);
        putchar('\n');
    }
}

