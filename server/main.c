
#include "command.h"

void *handle_data(void *fd);  //���������߳�

int main()
{
    int res;
    struct sockaddr_in s_addr, c_addr;     //����IP�Ͷ˿ںŵĽṹ
    int s_fd;            //������������

    int s_len, c_len;
    int *c_fd;            //�ͻ���������

     //�ṹ�ڴ��ʼ��
    bzero(&s_addr, sizeof(s_addr));
    bzero(&c_addr, sizeof(c_addr));

    s_len = sizeof(s_addr);
    c_len = sizeof(c_addr);

    s_addr.sin_family = AF_INET;                    //����ַ��
    s_addr.sin_port = htons(SOCK_PORT);            //���˿ں�
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);    //�Զ���ȡ����IP��ַ
    //inet_pton(AF_INET, "127.0.0.1", &s_addr.sin_addr);   //�ֶ�ָ��IP��ַ

    s_fd = socket(AF_INET, SOCK_STREAM, 0);                 //�����׽���

    res = bind(s_fd, (const struct sockaddr *)&s_addr, s_len);   //   ���׽���
    if(res == -1)
    {
        perror("bind failed!\n");
        exit(1);
    }

    //   �����׽���
    res = listen(s_fd, MAX_CONN_LIMIT);
    if(res == -1)
     {
         perror("listen error!\n");
         exit(1);
     }


    //***********�����׽��ֲ������߳�****************
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
        pthread_t pth;        //���̱�ʶ
        if (pthread_create(&pth, NULL, handle_data, c_fd))
        {
             fprintf(stderr, "pthread_create error!\n");
             break;
        }

    }

    close(s_fd);
    return 0;
}

 //    ���������߳�
void *handle_data(void *fd)
{
    int choose;                      //���տͻ��˵�ѡ��
    int run;
	int c_fd;

     run = 1;
     choose = 0;
     c_fd = *(int *)fd;

     printf("USER:%d connected...\n", c_fd);

     //****************����ͻ��˵�����******************
     while (run)
     {
        if(recv(c_fd, &choose, sizeof(choose), 0) <= 0)
        {
             puts("recv error...");
             break;
        }

        switch(choose)
        {
          case PUT:                         //����ͻ��������ļ�����
                   do_put(c_fd);
                   break;
          case GET:                         //����ͻ����ϴ��ļ�����
                   do_get(c_fd);
                   break;
          case CD:                          //����ͻ����л�Ŀ¼����
                   do_cd(c_fd);
                   break;
          case LS:                          //����ͻ����г�Ŀ¼��������
                   do_ls(c_fd);
	               break;
          case PWD:
                    do_pwd(c_fd);           //����ͻ�����ʾ��ǰĿ¼����
                    break;
          case BYE:                         //����ͻ��˶Ͽ���������
          default:
                   run = 0;
                   break;             //��ֹ�쳣�˳�
        }
     }

     close(c_fd);
     puts("connection closed...");
     pthread_exit(NULL);
}


