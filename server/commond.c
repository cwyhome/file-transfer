
#include"command.h"

unsigned long get_file_size(const char *filename)
{
	struct stat buf;
	if (stat(filename, &buf) < 0)
	{
         return -1;
	}
    return (unsigned long)buf.st_size;
}

int do_get(int sock_fd)
{
     int fd;            //�ļ�������
     int ok;           //�Ƿ�ִ�гɹ���־
     int sent;              //�շ��ֽ���
     char data_send[BUFFER_SIZE];    //���ͻ�����
     char data_recv[BUFFER_SIZE];    //���ջ�����
     char filename[MAX_FILENAME];     //�ļ���

     ok = 0;
     memset(data_send, 0, BUFFER_SIZE);
     memset(data_recv, 0, BUFFER_SIZE);
     memset(filename, 0, BUFFER_SIZE);

     if (sock_fd <= 0)
     {
         fprintf(stderr, "connection not established.\n");
         return -1;
     }

      recv(sock_fd, filename, BUFFER_SIZE, 0);  //��ȡ�ͻ��˷��͵��ļ���

      // ************�½��������ļ�***************
      fd = open(filename, O_RDWR|O_CREAT);
      //���ʧ�ܣ����͸��ͻ���״̬
      if (fd == -1)                  //�򿪻򴴽��ļ�ʧ��
      {
         ok = -1;
         fprintf(stderr, "error on create file.\n");
         send(sock_fd, &ok, sizeof(ok), 0);
         return -1;
      }
      ok = 1;
      send(sock_fd, &ok, sizeof(ok), 0);

      //    �ͻ��˴��ļ��ɹ�,���������,����ֹͣ����
      recv(sock_fd, &ok, sizeof(ok), 0);
      if (ok != 1)
      {
         puts("client stop file transfer...");
         return -1;
      }

     //  ���׽��ֶ�ȡһ�����ݲ�д���ļ�
     while (1)
     {
        recv(sock_fd, &sent, sizeof(sent), 0);
        if (sent == 0)
        {
           break;
        }
        recv(sock_fd, data_recv, sent,0);
        data_recv[sent] = 0;
        write(fd, data_recv, sent);
     }

     close(fd);    //�ر��ļ�������
     puts("receve file finished....");
     return 0;
}

int do_put(int sock_fd)
{
     int fd;                           //�ļ�������
     unsigned int filesize;          //�ļ���С
     int ok;                        //�Ƿ�ִ�гɹ���־
     int sent;                      //�շ��ֽ���
     char data_send[BUFFER_SIZE];    //���ͻ�����
     char data_recv[BUFFER_SIZE];    //���ջ�����
     char filename[MAX_FILENAME];
     ok = 0;
     filesize = 0;
     memset(data_send, 0, BUFFER_SIZE);
     memset(data_recv, 0, BUFFER_SIZE);
     memset(filename, 0, BUFFER_SIZE);

     if (sock_fd <= 0)
     {
         fprintf(stderr, "connection not established.\n");
         return -1;
     }

      recv(sock_fd, filename, BUFFER_SIZE, 0);   //��ȡ�ļ���

      //   ���ļ�׼������
      fd = open(filename, O_RDWR);
      if (fd == -1)                  //���ļ�ʧ��
      {
         fprintf(stderr, "error on open file.\n");
         ok = -1;
         send(sock_fd, &ok, sizeof(ok), 0);
         return -1;
      }
      ok = 1;                               //��ʾ�򿪳ɹ�����״̬���͸��ͻ���
      send(sock_fd, &ok, sizeof(ok), 0);

      filesize = get_file_size(filename);
      send(sock_fd, &filesize, strlen(data_send) + 1, 0);   //�����ļ���С

      recv(sock_fd, &ok, BUFFER_SIZE, 0);         //���տͻ��˵�״̬
      if (ok != 1)
      {
         puts("client stop download file...");
         close(fd);
         return -1;
      }

      while (1)  //������ȡ���ݲ�д���׽���  sent:�����ֽ���
	  {
          sent = read(fd, data_send, BUFFER_SIZE);
          send(sock_fd, &sent, sizeof(sent), 0);
          if(sent == 0)   break;
          send(sock_fd, data_send, sent, 0);
      }
      close(fd);
      puts("Transport finished...");
      return 0;
}

int do_cd(int sock_fd)
{
   char buff[BUFFER_SIZE];

   memset(buff, 0, BUFFER_SIZE);

   recv(sock_fd, buff, BUFFER_SIZE, 0);       //�����л�Ŀ¼·��
    if (chdir(buff) < 0)
    {
        //  �����ȡ��Ŀ¼�����ڣ�����ͻ��˷�����ʾ��Ϣ
        strcpy(buff, "Maybe path you inputnot exist...");
        send(sock_fd, buff, strlen(buff) + 1, 0);
        return -1;
    }
    else
    {
         strcpy(buff, getcwd(NULL, NULL));           //��ȡ��ǰ·��
         send(sock_fd, buff, strlen(buff) + 1, 0);   // �л����Ŀ¼��Ϣ���ظ��ͻ���
         printf("current dir:%s\n", buff);
    }

    return 0;
}

int do_ls(int sock_fd)   //��ȡĿ¼���ݲ����͵��׽���
{
    int ok;           //ͨ��״̬
    int sent;        //�շ��ֽ���
    DIR *dir;
	struct dirent *ptr;
    char data_send[BUFFER_SIZE];
    char path[MAX_FILENAME];

     ok = 0;
     bzero(path, MAX_FILENAME);

     recv(sock_fd, path, MAX_FILENAME, 0);    //��ȡĿ¼��

      if((dir = opendir(path)) == NULL)
      {
           ok = -1;
           send(sock_fd, &ok, sizeof(ok), 0);    //����״̬
           return -1;
      }
      ok = 1;
      send(sock_fd, &ok, sizeof(ok), 0);    //����״̬

      while (1)  //������ȡĿ¼���ݲ�д���׽���  sent:�����ֽ���
	  {
          ptr = readdir(dir);
          if(ptr == NULL)  sent = 0;
          else  sent = strlen(ptr->d_name);
          send(sock_fd, &sent, sizeof(sent), 0);
          if(sent == 0)   break;
          strcpy(data_send, ptr->d_name);
          puts(data_send);
          send(sock_fd, data_send, sent, 0);
      }
      closedir(dir);                        //�ر�Ŀ¼���
      printf("End ls from %d\n", sock_fd);
      return 0;
}

int do_pwd(int sock_fd)                     //��ʾ���ص�ǰ·��
{
     char dir[BUFFER_SIZE];
     printf("current dir: %s\n", getcwd(NULL, NULL));
     strcpy(dir, getcwd(NULL, NULL));          //��ȡ����˵�ǰĿ¼
     send(sock_fd, dir, strlen(dir) + 1, 0);   //���ͷ���˵�ǰĿ¼
     return 0;
}
