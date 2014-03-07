#include<stdio.h>
#include<unistd.h>


int main(int argc,char **argv)
{
    int ch;
    opterr = 1;  //是否让getoption内部打印错误

    while((ch = getopt(argc,argv,"a:bcde")) != -1)
    {
        printf("optind = %d\n", optind); //argv 参数的位置
        switch(ch)
        {
            case 'a': printf("option a :%s\n",optarg); break;
            case 'b': printf("option b :b\n"); break;
            case '?': printf("unknown option - %c \n", optopt);break; //如果遇到未知选项，打印出选项内容
            default:  printf("other option %c\n", ch);
        }
    }

}
