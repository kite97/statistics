typedef struct{
    char frame_head;
    char buf[];
}frame;

char   file_head[19];                               //存放文件头
char    channel_number;                             //声道数
