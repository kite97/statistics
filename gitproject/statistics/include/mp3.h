#ifndef _MP3_H_
#define _MP3_H_

#ifdef __cplusplus
extern "C" {
#endif

#define   ID3V1_size     128
int   bHasID3V2;
char   cVer;
char   cRevision;
int   bHasExtHeader = 0;
int   ID3V2_size;
char   cID3V2_head[10];
char   cID3v2Frame_head[10];
char   *cID3v2Frame;
long     iID3v2Frame_length;
long   iFrameStart, iFrameMove = 0, iFrameCount = 0;
unsigned   long   iFrameSize = 0;
char   cFrameHead_bin[33];
char   cFrameHead_bin_temp[9];
char   cID3V1[128];
unsigned   char*   cBuffer;
int   SamplingrateTable[3][3] = { \
    {44100,22050,11025},	\
    {48000,24000,12000},	\
    {32000,16000,8000} };
char*   ChannelDescrip[4] = { "stereo","Mixed stereo","Double track","mono" };
float  length;
int    cBuffer_size = 1024, Bitrate, FrameSize, FrameCount, mp3Duration, bSampleRate;
char   LayerDescript, bRateIndex, Version, bPadding, bChannelMode;
long   int   flength;
int   i = 0, j = 0;

void   getBin(int   dec, char*   bin);
int   GetBitRate(int   bRateIndex, int   LayerDescript, int   Version);
#ifdef __cplusplus
}
#endif

#endif
