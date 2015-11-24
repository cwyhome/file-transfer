
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
<<<<<<< HEAD
     int fd;            //文件描述符      
     int ok;            //确认标识
     int sent;              //收发字节数
     char data_send[BUFFER_SIZE];    //发送缓冲区
     char data_recv[BUFFER_SIZE];    //接收缓冲区
     char filename[MAX_FILENAME];    
     struct timeval timeout = {1, 0};  //超时时间为1秒，第二个参数单位为毫秒
=======
     int fd;            //文件描述符
     int ok;           //是否执行成功标志
     int sent;              //收发字节数
     char data_send[BUFFER_SIZE];    //发送缓冲区
     char data_recv[BUFFER_SIZE];    //接收缓冲区
     char filename[MAX_FILENAME];     //文件名

>>>>>>> 1870692faf2221a0d03cde912568a4978363990c
     ok = 0;
     memset(data_send, 0, BUFFER_SIZE);
     memset(data_recv, 0, BUFFER_SIZE);
     memset(filename, 0, BUFFER_SIZE);

     if (sock_fd <= 0)
     {
         fprintf(stderr, "connection not established.\n");
         return -1;
     }

<<<<<<< HEAD
      recv(sock_fd, filename, BUFFER_SIZE, 0);  //获取文件名

     
      fd = open(filename, O_RDWR|O_CREAT);     //打开并创建文件
 
      if (fd == -1)           //如果创建文件失败，则返回ok=-1            
=======
      recv(sock_fd, filename, BUFFER_SIZE, 0);  //读取客户端发送的文件名

      // ************新建并传输文件***************
      fd = open(filename, O_RDWR|O_CREAT);
      //如果失败，发送给客户端状态
      if (fd == -1)                  //打开或创建文件失败
>>>>>>> 1870692faf2221a0d03cde912568a4978363990c
      {
         ok = -1;
         fprintf(stderr, "error on create file.\n");
         send(sock_fd, &ok, sizeof(ok), 0);
         return -1;
      }
      ok = 1;
<<<<<<< HEAD
      send(sock_fd, &ok, sizeof(ok), 0); //创建文件成功，则返回ok=1
    
      recv(sock_fd, &ok, sizeof(ok), 0); //接收客户端的确认:ok=1
=======
      send(sock_fd, &ok, sizeof(ok), 0);

      //    客户端打开文件成功,则继续传输,否则停止传输
      recv(sock_fd, &ok, sizeof(ok), 0);
>>>>>>> 1870692faf2221a0d03cde912568a4978363990c
      if (ok != 1)
      {
         puts("client stop file transfer...");
         return -1;
      }
    
	 //    设置接收时延   超时则停止接收，recv返回-1
     setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

<<<<<<< HEAD
     //   开始文件传输
     while ((sent = recv(sock_fd, data_recv, BUFFER_SIZE, 0)) > 0)
=======
     //  从套接字读取一段数据并写入文件
     while (1)
>>>>>>> 1870692faf2221a0d03cde912568a4978363990c
     {
        if (write(fd, data_recv, sent) < 0)
        {
              return -1;
        }
     }

<<<<<<< HEAD
     close(fd);
=======
     close(fd);    //关闭文件描述符
>>>>>>> 1870692faf2221a0d03cde912568a4978363990c
     puts("receve file finished....");
     return 0;
}

int do_put(int sock_fd)
{
     int fd;                           //文件描述符
<<<<<<< HEAD
     unsigned int filesize;          //文件大小，单位为B
     int ok;                        //收发标识
     int sent;                      //收发字节
     char data_send[BUFFER_SIZE];    //发送缓冲
     char data_recv[BUFFER_SIZE];    //接受缓冲
     char filename[MAX_FILENAME];    //文件名
=======
     unsigned int filesize;          //文件大小
     int ok;                        //是否执行成功标志
     int sent;                      //收发字节数
     char data_send[BUFFER_SIZE];    //发送缓冲区
     char data_recv[BUFFER_SIZE];    //接收缓冲区
     char filename[MAX_FILENAME];
>>>>>>> 1870692faf2221a0d03cde912568a4978363990c
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

<<<<<<< HEAD
      recv(sock_fd, filename, BUFFER_SIZE, 0);   //接收文件名

      fd = open(filename, O_RDWR);        //打开文件
      if (fd == -1)                  //如果打开文件失败，则给客户端返回ok=-1
=======
      recv(sock_fd, filename, BUFFER_SIZE, 0);   //获取文件名

      //   打开文件准备传输
      fd = open(filename, O_RDWR);
      if (fd == -1)                  //打开文件失败
>>>>>>> 1870692faf2221a0d03cde912568a4978363990c
      {
         fprintf(stderr, "error on open file.\n");
         ok = -1;
         send(sock_fd, &ok, sizeof(ok), 0);
         return -1;
      }
<<<<<<< HEAD
      ok = 1;                               //如果打开文件失败，则给客户端返回ok=1
      send(sock_fd, &ok, sizeof(ok), 0);

      filesize = get_file_size(filename);    //获取文件大小
      send(sock_fd, &filesize, sizeof(filesize), 0);   //发送文件大小

      recv(sock_fd, &ok, BUFFER_SIZE, 0);         //接收客户端的确认
=======
      ok = 1;                               //表示打开成功，将状态发送给客户端
      send(sock_fd, &ok, sizeof(ok), 0);

      filesize = get_file_size(filename);
      send(sock_fd, &filesize, sizeof(filesize), 0);   //发送文件大小

      recv(sock_fd, &ok, BUFFER_SIZE, 0);         //接收客户端的状态
>>>>>>> 1870692faf2221a0d03cde912568a4978363990c
      if (ok != 1)
      {
         puts("client stop download file...");
         close(fd);
         return -1;
      }

<<<<<<< HEAD
     //    开始文件传输
     while ((sent = read(fd, data_send, BUFFER_SIZE)) > 0) 
     {
=======
      while (1)  //逐条读取内容并写入套接字  sent:发送字节数
	  {
          sent = read(fd, data_send, BUFFER_SIZE);
          send(sock_fd, &sent, sizeof(sent), 0);
          if(sent == 0)   break;
>>>>>>> 1870692faf2221a0d03cde912568a4978363990c
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

<<<<<<< HEAD
   recv(sock_fd, buff, BUFFER_SIZE, 0);       //接收客户切换路径名
    if (chdir(buff) < 0)
    {
=======
   recv(sock_fd, buff, BUFFER_SIZE, 0);       //接收切换目录路径
    if (chdir(buff) < 0)
    {
        //  请求读取的目录不存在，则给客户端返回提示信息
>>>>>>> 1870692faf2221a0d03cde912568a4978363990c
        strcpy(buff, "Maybe path you inputnot exist...");
        send(sock_fd, buff, strlen(buff) + 1, 0);
        return -1;
    }
    else
    {
<<<<<<< HEAD
         strcpy(buff, getcwd((char *)NULL, 1));           //获取当前目录，并存入buff
         send(sock_fd, buff, strlen(buff) + 1, 0);   // 发送当前目录
=======
         strcpy(buff, getcwd(NULL, NULL));           //获取当前路径
         send(sock_fd, buff, strlen(buff) + 1, 0);   // 切换后的目录信息返回给客户端
>>>>>>> 1870692faf2221a0d03cde912568a4978363990c
         printf("current dir:%s\n", buff);
    }

    return 0;
}

<<<<<<< HEAD
int do_ls(int sock_fd)   
{
    int ok;           //收发标识
=======
int do_ls(int sock_fd)   //读取目录内容并发送到套接字
{
    int ok;           //通信状态
>>>>>>> 1870692faf2221a0d03cde912568a4978363990c
    int sent;        //收发字节数
    DIR *dir;
	struct dirent *ptr;
    char data_send[BUFFER_SIZE];
    char path[MAX_FILENAME];

     ok = 0;
     bzero(path, MAX_FILENAME);

<<<<<<< HEAD
     recv(sock_fd, path, MAX_FILENAME, 0);    //接收客户端请求路径
=======
     recv(sock_fd, path, MAX_FILENAME, 0);    //获取目录名
>>>>>>> 1870692faf2221a0d03cde912568a4978363990c

      if((dir = opendir(path)) == NULL)
      {
           ok = -1;
<<<<<<< HEAD
           send(sock_fd, &ok, sizeof(ok), 0);    //打开路径失败则向客户端发送ok=-1
           return -1;
      }
      ok = 1;
      send(sock_fd, &ok, sizeof(ok), 0);    //打开路径成功则向客户端发送ok=1

      while (1)  //逐条读取目录内容并发送给客户端
=======
           send(sock_fd, &ok, sizeof(ok), 0);    //发送状态
           return -1;
      }
      ok = 1;
      send(sock_fd, &ok, sizeof(ok), 0);    //发送状态

      while (1)  //逐条读取目录内容并写入套接字  sent:发送字节数
>>>>>>> 1870692faf2221a0d03cde912568a4978363990c
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

<<<<<<< HEAD
int do_pwd(int sock_fd)                     
{
     char dir[BUFFER_SIZE];
     printf("current dir: %s\n", getcwd((char *)NULL, 0));
     strcpy(dir, getcwd((char *)NULL, 0));          //当前目录存入dir
     send(sock_fd, dir, strlen(dir) + 1, 0);   //发送当前目录
=======
int do_pwd(int sock_fd)                     //显示本地当前路径
{
     char dir[BUFFER_SIZE];
     printf("current dir: %s\n", getcwd(NULL, NULL));
     strcpy(dir, getcwd(NULL, NULL));          //获取服务端当前目录
     send(sock_fd, dir, strlen(dir) + 1, 0);   //发送服务端当前目录
>>>>>>> 1870692faf2221a0d03cde912568a4978363990c
     return 0;
}
