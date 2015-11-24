#include"command.h"
#include"input.h"

int sockfd;                       //套接字描述符
struct sockaddr_in s_addr_in;   //包含IP和端口信息的结构
command_line cmd;                //命令结构
char cmdline[MAX_LINE];          //输入命令行

int main()
{
   int choose;           //选项
   showuse();            //帮助菜单
   while (1)
   {
      printf("\nPlease input command:\n");
      cmdinit(&cmd);                        //初始化命令结构
      fgets(cmdline, 1024, stdin);
      split(&cmd, cmdline);

      //  判断输入命令是否为空
      if (cmd.name == NULL)
      {
         printf("no command...\n");
         continue;
      }

      switch(matchcmd(cmd.name))
      {
         case GET:
               choose = 1;
               send(sockfd, &choose, sizeof(choose), 0);    //给服务端发送命令代码
               do_get(cmd.argv[1], cmd.argv[2], sockfd);
               break;
         case PUT:
               choose = 2;
               send(sockfd, &choose, sizeof(choose), 0);    //给服务端发送命令代码
               do_put(cmd.argv[1], cmd.argv[2], sockfd);
               break;
         case SCD:
               choose = 3;
               send(sockfd, &choose, sizeof(choose), 0);    //给服务端发送命令代码
               do_serv_cd(cmd.argv[1], sockfd);   //进入远端服务器指定目录
               break;
         case SLS:
               choose = 4;
               send(sockfd, &choose, sizeof(choose), 0);    //给服务端发送命令代码
               do_serv_ls(cmd.argv[1], sockfd);     //列出当前服务器目录信息
               break;
         case CD:
               do_cd(cmd.argv[1]);           //进入当前主机指定目录
               break;
         case LS:
               do_ls(&cmd);                 //列出当前主机指定目录下的内容
               break;
         case PWD:                          //显示客户端当前目录
               do_pwd();
               break;
         case SPWD:                         //显示服务端当前目录
               choose = 5;
               send(sockfd, &choose, sizeof(choose), 0);    //给服务端发送命令代码
               do_serv_pwd(sockfd);
               break;
         case BYE:
               choose = 6;
               send(sockfd, &choose, sizeof(choose), 0);    //给服务端发送命令代码
               bye(sockfd);                 //退出服务器，关闭连接
               return 0;
         case CONNECT:
               if (do_connect(cmd.argv[1], &sockfd) == -1)
               {
                 fprintf(stderr, "connect server failed!\n");
                 break;
               }
               puts("------connect successfaully!-----");
               break;
         case HELP:
               showuse();
               break;
         case NO_MATCH:
               puts("command not found...");
               break;
         case EXIT:
               close(sockfd);
               return 0;
      }

      freeMemory(&cmd);    //释放命令结构内存
   }
}
