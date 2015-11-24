
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
     int fd;            
     int ok;           
     int sent;              
     char data_send[BUFFER_SIZE];    
     char data_recv[BUFFER_SIZE];    
     char filename[MAX_FILENAME];    
     struct timeval timeout = {1, 0};
     ok = 0;
     memset(data_send, 0, BUFFER_SIZE);
     memset(data_recv, 0, BUFFER_SIZE);
     memset(filename, 0, BUFFER_SIZE);

     if (sock_fd <= 0)
     {
         fprintf(stderr, "connection not established.\n");
         return -1;
     }

      recv(sock_fd, filename, BUFFER_SIZE, 0);  

 
      fd = open(filename, O_RDWR|O_CREAT);
 
      if (fd == -1)                
      {
         ok = -1;
         fprintf(stderr, "error on create file.\n");
         send(sock_fd, &ok, sizeof(ok), 0);
         return -1;
      }
      ok = 1;
      send(sock_fd, &ok, sizeof(ok), 0);
    
      recv(sock_fd, &ok, sizeof(ok), 0);
      if (ok != 1)
      {
         puts("client stop file transfer...");
         return -1;
      }
    
     setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

     while ((sent = recv(sock_fd, data_recv, BUFFER_SIZE, 0)) > 0)
     {
        if (write(fd, data_recv, sent) < 0)
        {
              return -1;
        }
     }

     close(fd);
     puts("receve file finished....");
     return 0;
}

int do_put(int sock_fd)
{
     int fd;                           //ÎÄŒþÃèÊö·û
     unsigned int filesize;          //ÎÄŒþŽóÐ¡
     int ok;                        //ÊÇ·ñÖŽÐÐ³É¹Š±êÖŸ
     int sent;                      //ÊÕ·¢×ÖœÚÊý
     char data_send[BUFFER_SIZE];    //·¢ËÍ»º³åÇø
     char data_recv[BUFFER_SIZE];    //œÓÊÕ»º³åÇø
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

      recv(sock_fd, filename, BUFFER_SIZE, 0);   //»ñÈ¡ÎÄŒþÃû

      //   openfile
      fd = open(filename, O_RDWR);
      if (fd == -1)                  //Žò¿ªÎÄŒþÊ§°Ü
      {
         fprintf(stderr, "error on open file.\n");
         ok = -1;
         send(sock_fd, &ok, sizeof(ok), 0);
         return -1;
      }
      ok = 1;                               //±íÊŸŽò¿ª³É¹Š£¬œ«×ŽÌ¬·¢ËÍžø¿Í»§¶Ë
      send(sock_fd, &ok, sizeof(ok), 0);

      filesize = get_file_size(filename);
      send(sock_fd, &filesize, sizeof(filesize), 0);   //·¢ËÍÎÄŒþŽóÐ¡

      recv(sock_fd, &ok, BUFFER_SIZE, 0);         //œÓÊÕ¿Í»§¶ËµÄ×ŽÌ¬
      if (ok != 1)
      {
         puts("client stop download file...");
         close(fd);
         return -1;
      }

     while ((sent = read(fd, data_send, BUFFER_SIZE)) > 0) 
     {
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

   recv(sock_fd, buff, BUFFER_SIZE, 0);       //œÓÊÕÇÐ»»Ä¿ÂŒÂ·Ÿ¶
    if (chdir(buff) < 0)
    {
        strcpy(buff, "Maybe path you inputnot exist...");
        send(sock_fd, buff, strlen(buff) + 1, 0);
        return -1;
    }
    else
    {
         strcpy(buff, getcwd((char *)NULL, 1));           //»ñÈ¡µ±Ç°Â·Ÿ¶
         send(sock_fd, buff, strlen(buff) + 1, 0);   // ÇÐ»»ºóµÄÄ¿ÂŒÐÅÏ¢·µ»Øžø¿Í»§¶Ë
         printf("current dir:%s\n", buff);
    }

    return 0;
}

int do_ls(int sock_fd)   //¶ÁÈ¡Ä¿ÂŒÄÚÈÝ²¢·¢ËÍµœÌ×œÓ×Ö
{
    int ok;           //Íš¬
    int sent;        //ÊÕ·¢×ÖœÚÊý
    DIR *dir;
	struct dirent *ptr;
    char data_send[BUFFER_SIZE];
    char path[MAX_FILENAME];

     ok = 0;
     bzero(path, MAX_FILENAME);

     recv(sock_fd, path, MAX_FILENAME, 0);    //»ñÈ¡Ä¿ÂŒÃû

      if((dir = opendir(path)) == NULL)
      {
           ok = -1;
           send(sock_fd, &ok, sizeof(ok), 0);    //·¢ËÍ×ŽÌ¬
           return -1;
      }
      ok = 1;
      send(sock_fd, &ok, sizeof(ok), 0);    //·¢ËÍ×ŽÌ¬

      while (1)  //ÖðÌõ¶ÁÈ¡Ä¿ÂŒÄÚÈÝ²¢ÐŽÈëÌ×œÓ×Ö  sent:·¢ËÍ×ÖœÚÊý
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
      closedir(dir);                        //¹Ø±ÕÄ¿ÂŒŸä±ú
      printf("End ls from %d\n", sock_fd);
      return 0;
}

int do_pwd(int sock_fd)                     //ÏÔÊŸ±ŸµØµ±Ç°Â·Ÿ¶
{
     char dir[BUFFER_SIZE];
     printf("current dir: %s\n", getcwd((char *)NULL, 0));
     strcpy(dir, getcwd((char *)NULL, 0));          //»ñÈ¡·þÎñ¶Ëµ±Ç°Ä¿ÂŒ
     send(sock_fd, dir, strlen(dir) + 1, 0);   //·¢ËÍ·þÎñ¶Ëµ±Ç°Ä¿ÂŒ
     return 0;
}
