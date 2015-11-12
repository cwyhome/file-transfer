
#include"command.h"
#include"input.h"
/***************************************
*
*  文件名:input.c
*  此模块初始化命令结构，并负责命令解析
*
****************************************/
void cmdinit(command_line *cmd)
{
     cmd->name = NULL;
     bzero(cmd->argv, sizeof(cmd->argv));
}

int split(command_line *cl, char line[])    //拆分命令相关函数
{
     int i = 0;
     int cmdLength = 0;
     char *token = NULL;
     char seps[] = " ";

     cmdLength = strlen(line);
     if (strlen(line) > COMMAND_LINE)
     {
         printf("the command is too long!\n");
         return -1;
     }

     /*********************************
       *
       * 由于fgets函数连同回车符也读取
       * 所以需要将读入的回车符号去掉
       *
     *********************************/
     line[cmdLength - 1] = '\0';

     /*******************************
      *
      * 通过strtok切割函数来分别
      * 获取命令名和参数t
      *
      *******************************/
     token = strtok(line, seps);
     if (token == NULL)
     {
         return -1;
     }

     cmdLength = strlen(token);
     cl->name = (char *)malloc(cmdLength + 1);
     if (cl->name == NULL)
     {
         perror("malloc failed");
         return -1;
     }
     strcpy(cl->name, token);              //获取命令名

    // 获取各命令参数
     while (token != NULL)
     {
         cmdLength = strlen(token);
         if (cmdLength > MAX_LINE)
         {
             printf("arg:%s is too long!\n", token);
             return -1;
         }

         cl->argv[i] = (char *)malloc(cmdLength + 1);

         if (cl->argv[i] == NULL)
         {
             perror("malloc failed");
             return -1;
         }

         strcpy(cl->argv[i], token);
         i++;
         token = strtok(NULL, seps);
     }
     return 0;
}
