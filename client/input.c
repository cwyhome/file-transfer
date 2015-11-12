
#include"command.h"
#include"input.h"
/***************************************
*
*  �ļ���:input.c
*  ��ģ���ʼ������ṹ���������������
*
****************************************/
void cmdinit(command_line *cmd)
{
     cmd->name = NULL;
     bzero(cmd->argv, sizeof(cmd->argv));
}

int split(command_line *cl, char line[])    //���������غ���
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
       * ����fgets������ͬ�س���Ҳ��ȡ
       * ������Ҫ������Ļس�����ȥ��
       *
     *********************************/
     line[cmdLength - 1] = '\0';

     /*******************************
      *
      * ͨ��strtok�и�����ֱ�
      * ��ȡ�������Ͳ���t
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
     strcpy(cl->name, token);              //��ȡ������

    // ��ȡ���������
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
