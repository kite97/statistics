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
char*  do_ls(char dirname[]){
    char* get_name;
    static char have_name[1024];
    DIR *dir_ptr;
    struct dirent *direntp;
    dir_ptr = opendir(dirname);
    if(dir_ptr == NULL){
        printf("Ls: can not open %s\n",dirname);
    }
    else{
        while(direntp = readdir(dir_ptr)){
            if(strcmp(direntp->d_name, "..") == 0 || strcmp(direntp->d_name,".") == 0){
                continue;
            }
            else if(strstr(have_name,direntp->d_name)){
                continue;
            }
            else{
                strcat(have_name, direntp->d_name);
                get_name = direntp->d_name;
                closedir(dir_ptr);
                return get_name;
            }
        }
    }
}

//主程序
int main(int argc, char* argv[]) {
    static char buff[1024];
    char* file_name;
    if(argc == 1){
        printf("There is no file\n");
    }
    else{
        for(; argc > 1; argc--){
            while(file_name = do_ls(argv[1])){
                printf("%s:\n",file_name);
                char address[64];
                sprintf(address,"%s/%s",argv[1],file_name);
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
        length_all = length_wave + length_mp3 +length_amr;
        printf("The length of wave is:\t%d s\n",length_wave);
        printf("The length of mp3 is:\t%d s\n",length_mp3);
        printf("The length of amr is:\t%d s\n",length_amr);
        printf("The length of all is:\t%d s\n",length_all);
        putchar('\n');
    }
}

