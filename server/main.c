
#include "command.h"
#include<errno.h>
#include <netdb.h>
void *handle_data(void *fd);  //ŽŠÀíÇëÇóÏß³Ì

int main()
{
    int res;
    struct sockaddr_in s_addr, c_addr;     //°üº¬IPºÍ¶Ë¿ÚºÅµÄœá¹¹
    int s_fd;            //·þÎñÆ÷ÃèÊö·û

    int s_len, c_len;
    int *c_fd;            //¿Í»§¶ËÃèÊö·û

     //œá¹¹ÄÚŽæ³õÊŒ»¯
    bzero(&s_addr, sizeof(s_addr));
    bzero(&c_addr, sizeof(c_addr));

    s_len = sizeof(s_addr);
    c_len = sizeof(c_addr);

    s_addr.sin_family = AF_INET;                    //Ìî³äµØÖ·×å
    s_addr.sin_port = htons(SOCK_PORT);            //Ìî³ä¶Ë¿ÚºÅ
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);    //×Ô¶¯»ñÈ¡±ŸµØIPµØÖ·
    //inet_pton(AF_INET, "127.0.0.1", &s_addr.sin_addr);   //ÊÖ¶¯Öž¶šIPµØÖ·

    s_fd = socket(AF_INET, SOCK_STREAM, 0);                 //œšÁ¢Ì×œÓ×Ö

    res = bind(s_fd, (const struct sockaddr *)&s_addr, s_len);   //   °ó¶šÌ×œÓ×Ö
    if(res == -1)
    {
        perror("bind failed!\n");
        exit(1);
    }

    //   ŒàÌýÌ×œÓ×Ö
    res = listen(s_fd, MAX_CONN_LIMIT);
    if(res == -1)
     {
         perror("listen error!\n");
         exit(1);
     }


    //***********ŒàÌýÌ×œÓ×Ö²¢ŽŽœšÏß³Ì****************
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
            printf("USER: %s connected...\n", IP);
        }
        else
        {
            continue;
        }

        pthread_t pth;        //œø³Ì±êÊ¶
        if (pthread_create(&pth, NULL, handle_data, c_fd))
        {
             fprintf(stderr, "pthread_create error!\n");
             break;
        }
    }

    close(s_fd);
    return 0;
}

 //    ŽŠÀíÇëÇóÏß³Ì
void *handle_data(void *fd)
{
    int choose;                      //œÓÊÕ¿Í»§¶ËµÄÑ¡Ôñ
    int run;
    int *c_fd;
      
     run = 1;
     choose = 0;
     c_fd = (int *)fd;

     //****************ŽŠÀí¿Í»§¶ËµÄÇëÇó******************
     while (run)
     {
        if(recv(*c_fd, &choose, sizeof(choose), 0) <= 0)
        {
             puts("recv error...");
             break;
        }

        switch(choose)
        {
          case PUT:                         //ŽŠÀí¿Í»§¶ËÏÂÔØÎÄŒþÇëÇó
                   do_put(*c_fd);
                   break;
          case GET:                         //ŽŠÀí¿Í»§¶ËÉÏŽ«ÎÄŒþÇëÇó
                   do_get(*c_fd);
                   break;
          case CD:                          //ŽŠÀí¿Í»§¶ËÇÐ»»Ä¿ÂŒÇëÇó
                   do_cd(*c_fd);
                   break;
          case LS:                          //ŽŠÀí¿Í»§¶ËÁÐ³öÄ¿ÂŒÄÚÈÝÇëÇó
                   do_ls(*c_fd);
	               break;
          case PWD:
                    do_pwd(*c_fd);           //ŽŠÀí¿Í»§¶ËÏÔÊŸµ±Ç°Ä¿ÂŒÇëÇó
                    break;
          case BYE:                         //ŽŠÀí¿Í»§¶Ë¶Ï¿ªÁ¬œÓÇëÇó
          default:
                   run = 0;
                   break;             //·ÀÖ¹Òì³£ÍË³ö
        }
     }

     close(*c_fd);
     free(c_fd);   //释放主线程动态内存
     c_fd = NULL;
     puts("connection closed...");
     pthread_exit(NULL);
}


