
#include "command.h"

void *handle_data(void *fd);  //处理请求线程

int main()
{
    int res;
    struct sockaddr_in s_addr, c_addr;     //包含IP和端口号的结构
    int s_fd;            //服务器描述符

    int s_len, c_len;
    int *c_fd;            //客户端描述符

     //结构内存初始化
    bzero(&s_addr, sizeof(s_addr));
    bzero(&c_addr, sizeof(c_addr));

    s_len = sizeof(s_addr);
    c_len = sizeof(c_addr);

    s_addr.sin_family = AF_INET;                    //填充地址族
    s_addr.sin_port = htons(SOCK_PORT);            //填充端口号
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);    //自动获取本地IP地址
    //inet_pton(AF_INET, "127.0.0.1", &s_addr.sin_addr);   //手动指定IP地址

    s_fd = socket(AF_INET, SOCK_STREAM, 0);                 //建立套接字

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


    //***********监听套接字并创建线程****************
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

//        char *IP = inet_ntoa(c_addr.sin_addr);
//        if (IP != NULL)
//        printf("USER NAME: %s\n", IP);
//        else
//        puts("get IP failed...");
        pthread_t pth;        //进程标识
        if (pthread_create(&pth, NULL, handle_data, c_fd))
        {
             fprintf(stderr, "pthread_create error!\n");
             break;
        }

    }

    close(s_fd);
    return 0;
}

 //    处理请求线程
void *handle_data(void *fd)
{
    int choose;                      //接收客户端的选择
    int run;
	int c_fd;

     run = 1;
     choose = 0;
     c_fd = *(int *)fd;

     printf("USER:%d connected...\n", c_fd);

     //****************处理客户端的请求******************
     while (run)
     {
        if(recv(c_fd, &choose, sizeof(choose), 0) <= 0)
        {
             puts("recv error...");
             break;
        }

        switch(choose)
        {
          case PUT:                         //处理客户端下载文件请求
                   do_put(c_fd);
                   break;
          case GET:                         //处理客户端上传文件请求
                   do_get(c_fd);
                   break;
          case CD:                          //处理客户端切换目录请求
                   do_cd(c_fd);
                   break;
          case LS:                          //处理客户端列出目录内容请求
                   do_ls(c_fd);
	               break;
          case PWD:
                    do_pwd(c_fd);           //处理客户端显示当前目录请求
                    break;
          case BYE:                         //处理客户端断开连接请求
          default:
                   run = 0;
                   break;             //防止异常退出
        }
     }

     close(c_fd);
     puts("connection closed...");
     pthread_exit(NULL);
}


