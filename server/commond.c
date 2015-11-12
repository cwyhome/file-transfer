
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
     int fd;            //文件描述符
     int ok;           //是否执行成功标志
     int sent;              //收发字节数
     char data_send[BUFFER_SIZE];    //发送缓冲区
     char data_recv[BUFFER_SIZE];    //接收缓冲区
     char filename[MAX_FILENAME];     //文件名

     ok = 0;
     memset(data_send, 0, BUFFER_SIZE);
     memset(data_recv, 0, BUFFER_SIZE);
     memset(filename, 0, BUFFER_SIZE);

     if (sock_fd <= 0)
     {
         fprintf(stderr, "connection not established.\n");
         return -1;
     }

      recv(sock_fd, filename, BUFFER_SIZE, 0);  //读取客户端发送的文件名

      // ************新建并传输文件***************
      fd = open(filename, O_RDWR|O_CREAT);
      //如果失败，发送给客户端状态
      if (fd == -1)                  //打开或创建文件失败
      {
         ok = -1;
         fprintf(stderr, "error on create file.\n");
         send(sock_fd, &ok, sizeof(ok), 0);
         return -1;
      }
      ok = 1;
      send(sock_fd, &ok, sizeof(ok), 0);

      //    客户端打开文件成功,则继续传输,否则停止传输
      recv(sock_fd, &ok, sizeof(ok), 0);
      if (ok != 1)
      {
         puts("client stop file transfer...");
         return -1;
      }

     //  从套接字读取一段数据并写入文件
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

     close(fd);    //关闭文件描述符
     puts("receve file finished....");
     return 0;
}

int do_put(int sock_fd)
{
     int fd;                           //文件描述符
     unsigned int filesize;          //文件大小
     int ok;                        //是否执行成功标志
     int sent;                      //收发字节数
     char data_send[BUFFER_SIZE];    //发送缓冲区
     char data_recv[BUFFER_SIZE];    //接收缓冲区
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

      recv(sock_fd, filename, BUFFER_SIZE, 0);   //获取文件名

      //   打开文件准备传输
      fd = open(filename, O_RDWR);
      if (fd == -1)                  //打开文件失败
      {
         fprintf(stderr, "error on open file.\n");
         ok = -1;
         send(sock_fd, &ok, sizeof(ok), 0);
         return -1;
      }
      ok = 1;                               //表示打开成功，将状态发送给客户端
      send(sock_fd, &ok, sizeof(ok), 0);

      filesize = get_file_size(filename);
      send(sock_fd, &filesize, strlen(data_send) + 1, 0);   //发送文件大小

      recv(sock_fd, &ok, BUFFER_SIZE, 0);         //接收客户端的状态
      if (ok != 1)
      {
         puts("client stop download file...");
         close(fd);
         return -1;
      }

      while (1)  //逐条读取内容并写入套接字  sent:发送字节数
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

   recv(sock_fd, buff, BUFFER_SIZE, 0);       //接收切换目录路径
    if (chdir(buff) < 0)
    {
        //  请求读取的目录不存在，则给客户端返回提示信息
        strcpy(buff, "Maybe path you inputnot exist...");
        send(sock_fd, buff, strlen(buff) + 1, 0);
        return -1;
    }
    else
    {
         strcpy(buff, getcwd(NULL, NULL));           //获取当前路径
         send(sock_fd, buff, strlen(buff) + 1, 0);   // 切换后的目录信息返回给客户端
         printf("current dir:%s\n", buff);
    }

    return 0;
}

int do_ls(int sock_fd)   //读取目录内容并发送到套接字
{
    int ok;           //通信状态
    int sent;        //收发字节数
    DIR *dir;
	struct dirent *ptr;
    char data_send[BUFFER_SIZE];
    char path[MAX_FILENAME];

     ok = 0;
     bzero(path, MAX_FILENAME);

     recv(sock_fd, path, MAX_FILENAME, 0);    //获取目录名

      if((dir = opendir(path)) == NULL)
      {
           ok = -1;
           send(sock_fd, &ok, sizeof(ok), 0);    //发送状态
           return -1;
      }
      ok = 1;
      send(sock_fd, &ok, sizeof(ok), 0);    //发送状态

      while (1)  //逐条读取目录内容并写入套接字  sent:发送字节数
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
      closedir(dir);                        //关闭目录句柄
      printf("End ls from %d\n", sock_fd);
      return 0;
}

int do_pwd(int sock_fd)                     //显示本地当前路径
{
     char dir[BUFFER_SIZE];
     printf("current dir: %s\n", getcwd(NULL, NULL));
     strcpy(dir, getcwd(NULL, NULL));          //获取服务端当前目录
     send(sock_fd, dir, strlen(dir) + 1, 0);   //发送服务端当前目录
     return 0;
}
