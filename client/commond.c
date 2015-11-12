#include"command.h"
int do_connect(char *ip, int *sock_fd)
{
     int ok;
     char data_send[BUFFER_SIZE];
     char data_recv[BUFFER_SIZE];

     bzero(data_send, BUFFER_SIZE);
     bzero(data_recv, BUFFER_SIZE);

     if (ip == NULL || sock_fd == NULL)
     {
         fprintf(stderr, "Error!");
         return -1;
     }

     *sock_fd = socket(AF_INET, SOCK_STREAM, 0);
     if (*sock_fd <= 0)
     {
          fprintf(stderr, "socket error!\n");
          return -1;
     }

     bzero(&s_addr_in, sizeof(s_addr_in));

     //**********����Զ�̷�����IP�Ͷ˿���Ϣ�ṹ��ʼ��*********
     s_addr_in.sin_addr.s_addr = inet_addr(ip);
     s_addr_in.sin_family = AF_INET;
     s_addr_in.sin_port = htons(SOCK_PORT);

     //     �ͷ�������������
     ok = connect(*sock_fd, (struct sockaddr *)(&s_addr_in), sizeof(s_addr_in));
     if (ok == -1)
     {
         return -1;
     }

     return 0;
}

int bye(int sock_fd)
{
   close(sock_fd);
   printf("connection is closed....\n");
   return 0;
}

void showuse()
{
        printf("\t\tFile Transfer System\n");
        printf("           (C) Copr Team  2015-11   Version 1.0 \n" );
        printf("User commands:\n");
        printf("Example: command -option1 option2 ...\n");
        printf("-------------------------------------------------------------------------------\n");
        printf("\tSYNOPSIS\tDESCRIPTION\t\tOTHER\n");
        printf("-------------------------------------------------------------------------------\n");
        printf("\tget arg1 arg2\tdownload\t\targ1:src file arg2:local path\n" );
        printf("\tput arg1 arg2\tupload\t\t\targ1:src file arg2:local path\n");
        printf("\t!cd arg1      \tchange server dir  \targ1:server dir\n");
        printf("\t!ls arg1      \tlist server dir  \targ1:server dir\n");
        printf("\tconnect arg1 \tconnect server  \targ1:IP address\n");
        printf("\tcd  arg1      \tchange local dir  \targ1:local dir\n");
        printf("\tls  arg1      \tlist current dir  \targ1:localdir\n");
        printf("\tpwd           \tdisplay local path\n");
        printf("\t!pwd           \tdisplay server path\n");
        printf("\tbye           \tdisconnect\n");
        printf("\thelp          \thelp\n");
        printf("\texit          \tquit system\n");
        printf("-------------------------------------------------------------------------------\n");
        printf( "  Note:    AP(absolute path).\n" );
        printf("-------------------------------------------------------------------------------\n");
}

int matchcmd(char *cmdname)
{
     if (strcmp(cmdname, "get") == 0)         return 1;
     if (strcmp(cmdname, "put") == 0)         return 2;
     if (strcmp(cmdname, "!cd") == 0)         return 3;
     if (strcmp(cmdname, "!ls") == 0)         return 4;
     if (strcmp(cmdname, "cd")  == 0)         return 5;
     if (strcmp(cmdname, "ls")  == 0)         return 6;
     if (strcmp(cmdname, "pwd") == 0)        return 7;
     if (strcmp(cmdname, "!pwd") == 0)       return 8;
     if (strcmp(cmdname, "bye") == 0)         return 9;
     if (strcmp(cmdname, "connect") == 0)     return 10;
     if (strcmp(cmdname, "help") == 0)        return 11;
     if (strcmp(cmdname, "exit") == 0)        return 12;
     return 13;                    //û��ƥ��������򷵻�11
}

int do_cd(char *path)
{
     if (path == NULL)
     {
         fprintf(stderr, "you must input a path.\n");
         return -1;
     }

     if (chdir(path) < 0)
     {
         fprintf(stderr, "Mybe the path is not exist.\n");
         return -1;
     }

     printf("current working directory: %s\n", (char *)getcwd(NULL, 0));
     return 0;
}

int do_ls(command_line *pcmd)          //�г���ǰ����ָ��Ŀ¼�µ�����
{
     int status;
     if (pcmd == NULL)
     {
         fprintf(stderr, "command is NULL.");
         return -1;
     }

      //   �����ӽ��̣�����execl��������ִ������
      if (!fork())
      {
         if (execvp(pcmd->argv[0], pcmd->argv) < 0)
         {
             fprintf(stderr, "Error on execvp.");
         }
          exit(0);
      }

     wait(&status);

     return 0;
}

int do_serv_ls(char *path, int sock_fd) //�г���ǰ������Ŀ¼��Ϣ
{
     int ok;          //ͨ��״̬��ʶ
     int sent;        //�շ��ֽ���
     char data_send[BUFFER_SIZE];
     char data_recv[BUFFER_SIZE];

     if (sock_fd <= 0)
     {
         fprintf(stderr, "connection not established.\n");
         return -1;
     }
     if (path == NULL)
     {
         puts("you must input pathname");
         return -1;
     }

     strcpy(data_send,  path);
     send(sock_fd, data_send, strlen(data_send) + 1, 0);       //����Ҫ�г���Ŀ¼

     recv(sock_fd, &ok, sizeof(ok), 0);               //���շ���˵�״̬
     if(ok == -1)
     {
        puts("Error on ls:Mybe the path you input not exist...");
        return -1;
     }

     //   ��ʾ������Ŀ¼����
     bzero(data_recv, BUFFER_SIZE);
     while (1)
     {
        recv(sock_fd, &sent, sizeof(sent), 0);
        if(sent == 0)
        {
           break;
        }
        recv(sock_fd, data_recv, sent,0);
        data_recv[sent] = 0;
        puts(data_recv);
     }
     return 0;
}

int do_serv_cd(char *path, int sock_fd)   //����Զ�˷�����ָ��Ŀ¼
{
     char data_send[BUFFER_SIZE];
     char data_recv[BUFFER_SIZE];

     if (sock_fd <= 0)
     {
         fprintf(stderr, "connection not established.\n");
         return -1;
     }
     if (path == NULL)
     {
         puts("you must input pathname");
         return -1;
     }

     //�����л�·���������ؽ��
     strcpy(data_send, path);
     send(sock_fd, data_send, strlen(data_send) + 1, 0);
     recv(sock_fd, data_recv, BUFFER_SIZE, 0);
     printf("current dir:%s\n", data_recv);

     return 0;
}

int do_get(const char *src, const char *dst, int sock_fd) //��Զ�˷����������ļ�
{
     int fd;              //�ļ�������
     int ok;              //�Ƿ�ִ�гɹ���־
     int sent;            //�շ��ֽ���
     unsigned int filesize;       //�ļ���С
     char data_send[BUFFER_SIZE];
     char data_recv[BUFFER_SIZE];

     ok = 0;
     bzero(data_send, BUFFER_SIZE);
     bzero(data_recv, BUFFER_SIZE);

     if (sock_fd <= 0)
     {
         fprintf(stderr, "connection not established.\n");
         return -1;
     }

     if (src == NULL || dst == NULL)
     {
         fprintf(stderr, "path error.");
         return -1;
     }

     // 1.�ͻ��η�����������.
     strcpy(data_send, src);
     send(sock_fd, data_send, strlen(data_send) + 1, 0);

     // 2.����˷����ļ��Ĵ�С���ߴ�����.
     recv(sock_fd, &ok, sizeof(ok), 0);          //����ok״̬��Ϣ
     if (ok == -1)
     {
         puts("Get error:May be the filename not exitst...");
         return -1;
     }

     recv(sock_fd, &filesize, sizeof(filesize), 0);  //��ȡ�ļ���С,��λΪ�ֽ�

          //  ��ӡ�ļ���Ϣ
     printf("-----------------------------------------------------------\n");
     printf("\t%s\t\t\t\t| %.3fKB\n", src, filesize / 1000.0);
     printf("-----------------------------------------------------------\n\n");
     printf("Total download size:%.3fKB\n", filesize / 1000.0);

     if (filesize / 1000.0 > MAX_FILE_SIZE)      //����ļ�̫����ȡ������
     {
         ok = -1;
         puts("Sory, the file is too large.");
         send(sock_fd, &ok, sizeof(ok), 0);  //���Ϳͻ�״̬��Ϣ
         return -1;
     }

     ok = 1;
     send(sock_fd, &ok, sizeof(ok), 0);      //���Ϳͻ�״̬��Ϣ

     fd = open(dst, O_RDWR|O_CREAT);
     if (fd == -1)                  //�򿪻򴴽��ļ�ʧ��
     {
        fprintf(stderr, "error on create file.\n");
        return -1;
     }

     //  ���׽��ֶ�ȡһ�����ݲ�д���ļ�
     while (1)
     {
        recv(sock_fd, &sent, sizeof(sent), 0);
        if(sent == 0)
        {
           break;
        }
        recv(sock_fd, data_recv, sent,0);
        data_recv[sent] = 0;
        write(fd, data_recv, sent);
     }
     close(fd);
     printf("Download  finished...\n");
     return 0;
}

int do_put(const char *src, const char *dst, int sock_fd) //��Զ�˷������ϴ��ļ�
{
     int fd;              //�ļ�������
     int ok;              //�Ƿ�ִ�гɹ���־
     int sent;            //�շ��ֽ���
     char data_send[BUFFER_SIZE];
     char data_recv[BUFFER_SIZE];

     ok = 0;
     bzero(data_send, BUFFER_SIZE);
     bzero(data_recv, BUFFER_SIZE);

     if (sock_fd <= 0)
     {
         fprintf(stderr, "connection not established.\n");
         return -1;
     }

     if (src == NULL || dst == NULL)
     {
         fprintf(stderr, "path error.");
         return -1;
     }

       // 1.�ͻ��η�����������.
     strcpy(data_send, dst);
     send(sock_fd, data_send, strlen(data_send) + 1, 0);

       // 2.����˷���ok���ߴ�����.
     recv(sock_fd, &ok, sizeof(ok), 0);       //��ȡ״̬
     if (ok == -1)
     {
         puts("May be the server path unavailable...");
         return -1;
     }

      // 3.�ͻ��˷���ok��ʾ��Ҫ�����ļ�.
      fd = open(src, O_RDWR);
      if (fd == -1)                  //���ļ�ʧ��
      {
         ok = -1;
         fprintf(stderr, "error on open file.\n");
         send(sock_fd, &ok, sizeof(ok), 0);
         return -1;
      }
     ok = 1;
     send(sock_fd, &ok, sizeof(ok), 0);   //���Ϳͻ���״̬

       // 4.��ʼ�����ļ�
      while (1)  //������ȡ���ݲ�д���׽���  sent:�����ֽ���
	  {
          sent = read(fd, data_send, BUFFER_SIZE);
          send(sock_fd, &sent, sizeof(sent), 0);
          if(sent == 0)   break;
          send(sock_fd, data_send, sent, 0);
      }

     close(fd);   //�ر��ļ�������
     printf("upload finished...\n");
     return 0;
}

int do_pwd()                     //��ʾ���ص�ǰ·��
{
     printf("current client dir: %s\n", getcwd(NULL, NULL));
     return 0;
}

int do_serv_pwd(int sock_fd)    //��ʾ��ǰ����˵�·��
{
     char dir[BUFFER_SIZE] = {0};

     if (sock_fd <= 0)
     {
         puts("the connection not established...");
         return -1;
     }
     if (recv(sock_fd, dir, BUFFER_SIZE, 0) <= 0)
     {
         puts("cant get current dir...");
         return -1;
     }
     printf("current server dir:%s\n", dir);
     return 0;
}

void freeMemory(command_line *cmd)
{
     int i = 0;         //�����±�
     int cmdLength = 0;

     if (cmd != NULL)
     {
         cmdLength = sizeof(cmd->argv);
         if (cmd->name != NULL)
         {
             free(cmd->name);
             cmd->name = NULL;
         }

         while (cmd->argv[i] != NULL && i < cmdLength)
         {
             free(cmd->argv[i]);
             cmd->argv[i] = NULL;
             i++;
         }
     }
}







