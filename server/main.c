
#include "command.h"
#include<errno.h>
#include <netdb.h>
void *handle_data(void *fd);  //线程函数

int main()
{
    int res;
    struct sockaddr_in s_addr, c_addr;
    int s_fd;            //服务端套接字描述符

    int s_len, c_len;
    int *c_fd;            //客户端描述符指针

     //结构内存清零
    bzero(&s_addr, sizeof(s_addr));
    bzero(&c_addr, sizeof(c_addr));

    s_len = sizeof(s_addr);
    c_len = sizeof(c_addr);

    s_addr.sin_family = AF_INET;                    //TCP/IP地址族
    s_addr.sin_port = htons(SOCK_PORT);            //端口号
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);    //自动获取本机IP
    //inet_pton(AF_INET, "127.0.0.1", &s_addr.sin_addr);   //手动指定IP

    s_fd = socket(AF_INET, SOCK_STREAM, 0);                 //创建套接字描述符

    res = bind(s_fd, (const struct sockaddr *)&s_addr, s_len);   //   绑定套接字
    if(res == -1)
    {
        perror("bind failed!\n");
        exit(1);
    }

    //   监听套接字
    res = listen(s_fd, MAX_CONN_LIMIT);
    if(res == -1)
     {
         perror("listen error!\n");
         exit(1);
     }


    //************服务端循环监听***************
    while(1)
    {
        c_fd = (int *)malloc(sizeof(c_fd));
        if (c_fd == NULL)
        {
            puts("malloc memory failed...");
            exit(-1);
        }

        printf(">>:servert started,waiting for new connection...\n");
        *c_fd = accept(s_fd, (struct sockaddr *)&c_addr, &c_len);
        if(*c_fd < 0)
        {
            fprintf(stderr, "accept error...\n");
            continue;
        }

        char *IP = inet_ntoa(c_addr.sin_addr);     //显示用户名
        if (IP)
        {
            printf("USER: %s connected...\n", IP);  //打印连接到服务器客户端的IP
        }
        else
        {
            continue;
        }

        pthread_t pth;        //线程标识
        if (pthread_create(&pth, NULL, handle_data, c_fd))
        {
             fprintf(stderr, "pthread_create error!\n");
             break;
        }
    }

    close(s_fd);
    return 0;
}

 //    线程处理函数定义
void *handle_data(void *fd)
{
    int choose;                      //服务端处理不同请求标识
    int run;            
    int *c_fd;
      
     run = 1;
     choose = 0;
     c_fd = (int *)fd;

     //****************处理客户端请求******************
     while (run)
     {
        if(recv(*c_fd, &choose, sizeof(choose), 0) <= 0)
        {
             puts("recv error...");
             break;
        }

        switch(choose)
        {
          case PUT:                         //处理客户端下载请求
                   do_put(*c_fd);
                   break;
          case GET:                         //处理客户端上传请求
                   do_get(*c_fd);
                   break;
          case CD:                          //处理客户端切换目录请求
                   do_cd(*c_fd);
                   break;
          case LS:                          //处理客户端读取目录请求
                   do_ls(*c_fd);
	           break;
          case PWD:
                    do_pwd(*c_fd);           //处理客户端显示当前目录请求
                    break;
          case BYE:                         //处理客户端断开连接请求
          default:
                   run = 0;
                   break;                   //如果没有匹配到请求码，则结束线程
        }
     }

     close(*c_fd);
     free(c_fd);   //释放主线程动态内存
     c_fd = NULL;
     puts("connection closed...");
     pthread_exit(NULL);
}


