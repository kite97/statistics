#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "wave.h"

extern int length_wave;

bool wave_check_chunk(wave_t *wav) {
    if (!strncasecmp(wav->riff.chunk_id, "RIFF", 4)) {
        if (!strncasecmp(wav->riff.wave_id, "wave", 4)) {
            ;
        } else {
            return false;
        }
    }
    if (!strncasecmp(wav->fmt.chunk_id, "fmt", 3)) {
        if (wav->fmt.chunk_size == 16) {
            if (!strncasecmp(wav->type.pcm.chunk_id, "data", 4)) {
                return true;
            } else {
                return false;
            }
        } else if (wav->fmt.chunk_size == 18) {
            if (!strncasecmp(wav->type.law.fact.chunk_id, "fact", 4)) {
                if (!strncasecmp(wav->type.law.data.chunk_id, "data", 4)) {
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
    return true;
}

void wave_parse_chunk(wave_t *wav, FILE *fp, int *data_frame,int ms) {
    int data_offset;
    switch(wav->fmt.format_tag) {
        case WAVE_PCM :
            data_offset = (unsigned long)&wav->type.pcm.buf[0] - (unsigned long)wav;
            break;
        case WAVE_ALAW :
        case WAVE_MULAW :
            data_offset = (unsigned long)&wav->type.law.data.buf[0] - (unsigned long)wav;
            break;
        default :
            break;
    }
    fseek(fp, data_offset, SEEK_SET);
    *data_frame = wav->fmt.nchannels * wav->fmt.sample_rate * wav->fmt.bits_per_sample / 8 / 1000 * ms;
}

int wave_fread_frame(char *buf, int frame_size, FILE *fp) {
    int num;
    num = fread(buf, 1, frame_size, fp);
    if(num != frame_size) {
        if(num % 2) {
            return num+1;
        }
    }
    return num;
}

void display(wave_t* wav){
        int length = 0;
        printf("ChunkID \t%c%c%c%c\n", wav->riff.chunk_id[0], wav->riff.chunk_id[1], wav->riff.chunk_id[2], wav->riff.chunk_id[3]);
        printf("ChunkSize \t%d\n", wav->riff.chunk_size);
        printf("Format \t\t%c%c%c%c\n", wav->riff.wave_id[0], wav->riff.wave_id[1], wav->riff.wave_id[2], wav->riff.wave_id[3]);
        printf("SubchunkOneID \t%c%c%c%c\n", wav->fmt.chunk_id[0], wav->fmt.chunk_id[1], wav->fmt.chunk_id[2], wav->fmt.chunk_id[3]);
        printf("SubchunkOneSize %d\n", wav->fmt.chunk_size);
        printf("FormatTag \t%d\n", wav->fmt.format_tag);
        printf("NumChannels \t%d\n", wav->fmt.nchannels);
        printf("SampleRate \t%d\n", wav->fmt.sample_rate);
        printf("ByteRate \t%d\n", wav->fmt.bytes_rate);
        printf("BlockAlign \t%d\n", wav->fmt.block_align);
        printf("BitsPerSample \t%d\n", wav->fmt.bits_per_sample);
        int i = wav->fmt.format_tag;
        if(i == 1){
            printf("SubchunkTwoID \t%c%c%c%c\n", wav->type.pcm.chunk_id[0], wav->type.pcm.chunk_id[1], wav->type.pcm.chunk_id[2], wav->type.pcm.chunk_id[3]);
            printf("SubchunkTwoSize %d\n", wav->type.pcm.chunk_size);
            length = (int)(wav->type.pcm.chunk_size / wav->fmt.bytes_rate);
            printf("Length \t\t%d s\n\n\n",length);
            length_wave += length;
        }
        else{
            printf("CbSize \t\t%d\n", wav->type.law.fact.cb_size);
            printf("ChunkID \t%c%c%c%c\n", wav->type.law.fact.chunk_id[0], wav->type.law.fact.chunk_id[1], wav->type.law.fact.chunk_id[2], wav->type.law.fact.chunk_id[3]);
            printf("ChunkSize \t%d\n", wav->type.law.fact.chunk_size);
            printf("DwSampleLength \t%d\n", wav->type.law.fact.dw_sample_length);
            printf("SubchunkTwoID \t%c%c%c%c\n", wav->type.law.data.chunk_id[0], wav->type.law.data.chunk_id[1], wav->type.law.data.chunk_id[2], wav->type.law.data.chunk_id[3]);
            printf("SubchunkTwoSize %d\n", wav->type.law.data.chunk_size);
            length = (int)(wav->type.law.data.chunk_size / wav->fmt.bytes_rate);
            printf("Length \t\t%d s\n\n\n",length);
            length_wave += length;
        }
}


void wave(wave_t *wav ,FILE *fp){
    char buf[1024*4];
    display(wav);
   /* int frame_size;
    int ms;
    ms = 20;
    if (wave_check_chunk(wav)) {
        wave_parse_chunk(wav, fp, &frame_size,ms);
    } else {
        exit(-1);
    }
    int num;*/
}

