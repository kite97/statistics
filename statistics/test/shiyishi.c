#include <stdio.h>

#include <dirent.h>

int main()

{

    int filesize = 0;

    DIR *dirptr=NULL;

    int i=1;

    struct dirent *entry;

    if((dirptr = opendir("voice"))==NULL)

    {

        printf("opendir failed!");

        return 1;

    }else

    {

        while(entry=readdir(dirptr))

        {

            printf("filename%d = %s",i,entry->d_name);  //输出文件或者目录的名称，一般文件含格式名，目录没有格式名

            printf("filetype = %d\n",entry->d_type);  //输出文件类型

            i++;

        }

        closedir(dirptr);

    }

    return 0;

}

