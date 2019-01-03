#include <stdio.h>
#include <string.h>

int match(char *str1, char *str2)
{
    if(str1 == NULL || str2 == NULL)
        return 0 ;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    char *tmp_str = str2 ;
    int tmp_len = len2 ;
    int location = 0 ;
    int match_flag = 0 ;
    int i = 0 ;

    while(i<tmp_len) {
        if(tmp_str[i] == '*') {
            location = i+1 ;
            break ;
        }
        i++ ;
    }

    char *tmp_str1 = str1 ;
    char *tmp_str2 = str2 ;

    if(location) {
        location -= 1;
        //�ֶԱ�* ǰ���ַ����� ���ǰ�涼ûƥ�䣬ֱ�ӷ���δƥ����
        if(!strncmp(str1, str2, location)) {
            tmp_len = len2 - location ;
            tmp_str1 += len1 ;
            tmp_str2 += len2 ;

            //��ƥ��*����ַ�������β��ǰ�ҡ�
            while(tmp_len && (*tmp_str1 == *tmp_str2)){
                tmp_str1 -- ;
                tmp_str2 -- ;
                tmp_len -- ;
            }

            match_flag = tmp_len?0:1 ;
        }
    }

    return match_flag ;
}

int main(){
int i ;
i = match("1.wav","*.wav");
printf("%d\n",i);
}
