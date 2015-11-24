#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED
#include"common.h"
#define MAX_LINE       1024             //向服务器端发送命令的最大长度
#define COMMAND_LINE   256             //用户输入客户端命令的最大长度
#define MAX_ARG         64              //每个参数或命令的最大长度

typedef struct
{
    char *name;                   //命令的名称
    char *argv[MAX_ARG];          // 命令携带的参数
}command_line;

void cmdinit(command_line *cmd);  //命令结构初始化

int split(command_line *cl, char line[]);    //拆分命令相关函数

#endif // INPUT_H_INCLUDED
