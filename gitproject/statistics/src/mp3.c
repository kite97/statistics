#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "mp3.h"

extern int length_mp3;

void   mp3(FILE *fp)
{
    fread(cID3V2_head, 10, 1, fp);
    printf("ID3V2_head:\t%s\n", cID3V2_head);
    if ((cID3V2_head[0] == 'I' || cID3V2_head[1] == 'i')
            && (cID3V2_head[1] == 'D' || cID3V2_head[2] == 'd')
            && cID3V2_head[2] == '3') {
        bHasID3V2 = 1;
        cVer = cID3V2_head[3];
        cRevision = cID3V2_head[4];
        if (cID3V2_head[5] & 0x64 == 0x64)   bHasExtHeader = 1;
        if (cID3V2_head[5] & 0x10 == 0x10)   bHasExtHeader = 1;
        ID3V2_size = (cID3V2_head[6] & 0x7F) << 21
            | (cID3V2_head[7] & 0x7F) << 14
            | (cID3V2_head[8] & 0x7F) << 7
            | (cID3V2_head[9] & 0x7F) + 10;
        printf("ID3V2_size:\t%dbytes\n", ID3V2_size);
        iFrameStart = --ID3V2_size;
    }
    else {
        bHasID3V2 = 0;
        printf("\nThere   is   no   ID3V2*\n");
    }
    rewind(fp);
    fseek(fp, 10L, 1);
    while ((ftell(fp) + 10) <= ID3V2_size) {
        memset(cID3v2Frame_head, 0, 10);
        fread(cID3v2Frame_head, 10, 1, fp);
        iID3v2Frame_length = (long)(cID3v2Frame_head[4] * 0x100000000   \
                + cID3v2Frame_head[5] * 0x10000 + cID3v2Frame_head[6] * 0x100    \
                + cID3v2Frame_head[7]);
        if (cID3v2Frame_head[0] == '\0'
                &&   cID3v2Frame_head[1] == '\0'
                &&   cID3v2Frame_head[2] == '\0'
                &&   cID3v2Frame_head[3] == '\0')
            break;
        for (i = 0;i < 4;i++)
            putchar(cID3v2Frame_head[i]);     /*FrameSign(4)*/
        printf(":\t\t");  /*   Print   ":"*/
        cID3v2Frame = malloc(iID3v2Frame_length);
        if (cID3v2Frame != NULL) {
            fread(cID3v2Frame, iID3v2Frame_length, 1, fp);
            i = 0;
            while (i < iID3v2Frame_length) {
                putchar(cID3v2Frame[i]);
                i++;
            }
            putchar('\n');
            free(cID3v2Frame);
        }
        else {
            if (fp != NULL)   fclose(fp);
            printf("ERROR:Memory   Exhausted!");
            exit(1);
        }
    }
    fseek(fp, 0L, SEEK_END);
    flength = ftell(fp);
    rewind(fp);
    fseek(fp, 10 + ID3V2_size, 1);
    while ((ftell(fp) + 4) < flength) {
        if (ftell(fp) + 1024 <= flength)
            cBuffer_size = 1024;
        else
            cBuffer_size = flength - ftell(fp);
        cBuffer = malloc(cBuffer_size);
        if (cBuffer == NULL) {
            if (fp != NULL)   fclose(fp);
            printf("ERROR:Memory   Exhausted!");
            exit(1);
        }
        memset(cBuffer, 0, cBuffer_size);
        fread(cBuffer, cBuffer_size, 1, fp);
        for (i = 0; i < (cBuffer_size - 4); i++) {
            LayerDescript = (cBuffer[i + 1] & 0x6) >> 1;
            bRateIndex = cBuffer[i + 2] >> 4;
            bSampleRate = (cBuffer[i + 2] & 0xC) >> 2;
            if (cBuffer[i] == 0xFF
                    && cBuffer[i + 1] >= 0xE0
                    && bRateIndex != 0xF
                    && LayerDescript != 0x0
                    && bSampleRate < 0x3) {
                Version = (cBuffer[i + 1] & 0x18) >> 3;
                printf("Version:\t%d ",Version );
                bPadding = (cBuffer[i + 2] & 0x2) >> 1;
                Bitrate = GetBitRate(bRateIndex, 4 - LayerDescript, Version);
                bChannelMode = (cBuffer[i + 3] & 0xC0) >> 6;
                putchar('\n');
                if (bRateIndex != 0) {
                    switch (Version) {
                        case   0:
                            printf("MPEG:\t\t2.5\nlayer:\t\t%d\nSamplingrate:\t%dHz\nChannelDescrip:\t%s\n", 4 - LayerDescript, \
                                    SamplingrateTable[bSampleRate][2], \
                                    ChannelDescrip[bChannelMode]);
                            FrameSize = ((576/8 * Bitrate * 1000) / SamplingrateTable[bSampleRate][2]) + bPadding;
                            printf("FrameSize:\t%d\n" ,FrameSize);
                            FrameCount = (int)((flength - (10 + ID3V2_size) - ID3V1_size)/ FrameSize);
                            mp3Duration = (int)(flength*8/Bitrate/1000);
                            printf("CBR:\t\t%dKbit\n" , Bitrate);
                            printf("FrameCount:\t%dframes\n", FrameCount);
                            printf("Length:\t\t%d s\n", mp3Duration);
                            printf("\n");
                            break;
                        case   2:
                            printf("MPEG:\t\t2.0\nlayer:\t\t%d\nSamplingrate:\t%dHz\nChannelDescrip:\t%s\n", 4 - LayerDescript, \
                                    SamplingrateTable[bSampleRate][1], ChannelDescrip[bChannelMode]);
                            FrameSize = ((576/8 * Bitrate * 1000) / SamplingrateTable[bSampleRate][1]) + bPadding;
                            printf("FrameSize: \t%d\n" , FrameSize);
                            FrameCount = (int)((flength - (10 + ID3V2_size) - ID3V1_size)/ FrameSize);
                            mp3Duration = (int)(flength*8/Bitrate/1000);
                            printf("CBR:\t\t%dKbit\n" , Bitrate);
                            printf("FrameCount:\t%dframes\n", FrameCount);
                            printf("Length:\t\t%d s\n", mp3Duration);
                            printf("\n");
                            break;
                        case   3:
                            printf("MPEG:\t\t1.0\nlayer:\t\t%d\nSamplingrate:\t%dHz\nChannelDescrip:\t%s\n", 4 - LayerDescript, \
                                    SamplingrateTable[bSampleRate][0], \
                                    ChannelDescrip[bChannelMode]);
                            FrameSize = ((1152/8 * Bitrate * 1000) / SamplingrateTable[bSampleRate][0])  + bPadding;
                            printf("FrameSize: \t%d\n",  FrameSize);
                            FrameCount = (int)((flength - (10 + ID3V2_size) - ID3V1_size)/ FrameSize);
                            mp3Duration = (int)(flength*8/Bitrate/1000);
                            printf("CBR:\t\t%dKbit\n" , Bitrate);
                            printf("FrameCount:\t%dframes\n", FrameCount);
                            printf("Length:\t\t%d s\n", mp3Duration);
                            break;
                    }
                    length_mp3 += mp3Duration;
                }
                else
                    printf("This   a   Free   Rate   MP3   File!\n");
                cBuffer_size = -1;
                break;
            }
        }
        free(cBuffer);
        if (cBuffer_size == (-1))   break;
    }
    fseek(fp, -128L, SEEK_END);
    fread(cID3V1, ID3V1_size, 1, fp);
    if ((cID3V1[0] == 'T' || cID3V1[0] == 't')
            && (cID3V1[1] == 'A' || cID3V1[1] == 'a')
            && (cID3V1[2] == 'G' || cID3V1[2] == 'g')) {
        printf("ID3V1_size:\t%dbytes\n", ID3V1_size);
        printf("ID3V1:\t\t");
        for (i = 0;i < 128-1;i++)   putchar(cID3V1[i]);
    }
    else {
        printf("There   is   no   ID3V1*\n\n\n");
    }
    fclose(fp);
}

void   getBin(int   dec, char*   bin)
{
    int   i, j, len;
    char   temp[8];
    memset(temp, 0, 8);
    sprintf(temp, "%d", dec);
    len = strlen(temp);
    memset(bin, 0, strlen(bin));
    for (i = 0, j = 0;i < 8;i++, j++) {
        if (temp[j] != '\0')
            bin[i] = temp[j];
        else
            bin[i] = '0';
    }
    bin[i] = '\0';
    for (i = 0, j = 0;i < 8;i++, j++)   putchar(bin[i]);
}

int   GetBitRate(int   bRateIndex, int   LayerDescript, int   Version)
{
    int   BitrateTable[6][15] = {
        {-1,32,64,96,128,160,192,224,256,288,320,352,384,416,448},
        {-1,32,48,56,64,80,96,112,128,160,192,224,256,320,384},
        {-1,32,40,48,56,64,80,96,112,128,160,192,224,256,320},
        {-1,32,48,56,64,80,96,112,128,144,160,176,192,224,256},
        {-1,8,16,24,32,40,48,56,64,80,96,112,128,144,160},
        {-1,8,16,24,32,40,48,56,64,80,96,112,128,144,160}
    };         //kbps       (-1)   means   :free
    int   i, j;
    if (Version == 3 && LayerDescript == 1)j = 0  ;
    else if (Version == 3 && LayerDescript == 2)j = 1;
    else if (Version == 3 && LayerDescript == 3)j = 2;
    else if ((Version == 2 || Version == 0) && LayerDescript == 1)j = 3;
    else if ((Version == 2 || Version == 0) && LayerDescript == 2)j = 4;
    else if ((Version == 2 || Version == 0) && LayerDescript == 3)j = 5;
    i = BitrateTable[j][bRateIndex];
    return   i;
}

