#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED
#include"common.h"

#define SOCK_PORT       4321            //端口号
#define BUFFER_SIZE     1024            //缓冲区大小
#define MAX_FILENAME    256             //文件名最大长度
#define MAX_CONN_LIMIT 512             //服务的监听队列上限

typedef enum{PUT=1, GET, CD, LS, PWD, BYE}ENCODE;  //定义命令代码

 /***********************************
    *
    * 获取文件大小
    * filename:获取文件大小
    * 返回直:返回文件大小，出错则返回-1.
    *
  ***********************************/
unsigned long get_file_size(const char *filename); //获取文件大小

 /***********************************
    *
    * 处理用户put命令
    * sock_fd:套接字描述符
    * 返回直:成功返回0，否则返回-1
    *
  ***********************************/
 int do_get(int sock_fd);

  /***********************************
    *
    * 处理用户get命令
    * sock_fd:套接字描述符
    * 返回直:成功返回0，否则返回-1
    *
  ***********************************/
 int do_put(int sock_fd);

  /***********************************
    *
    * 处理用户的!cd请求
    * sock_fd:套接字描述符
    * 返回直:成功返回0，否则返回-1
    *
   ***********************************/
 int do_cd(int sock_fd);

  /***********************************
    *
    * 处理用户的!ls请求
    * sock_fd:套接字描述符
    * 返回直:成功返回0，否则返回-1
    *
   ***********************************/
 int do_ls(int sock_fd);

  /***********************************
    *
    * 处理用户的!pwd请求
    * sock_fd:套接字描述符
    * 返回直:成功返回0，否则返回-1
    *
   ***********************************/
 int do_pwd(int sock_fd);

#endif // COMMAND_H_INCLUDED
