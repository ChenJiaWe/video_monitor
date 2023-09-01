#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MAX_TEXT_SIZE 100

struct msg_buffer
{
  long msg_type;
  char msg_text[MAX_TEXT_SIZE];
};

int main()
{
  key_t key;
  int msgid;
  struct msg_buffer message;

  // 生成一个唯一的key
  if ((key = ftok("/tmp", 'a')) == -1)
  {
    perror("ftok");
    exit(1);
  }

  // 创建一个消息队列
  if ((msgid = msgget(key, 0666 | IPC_CREAT)) == -1)
  {
    perror("msgget");
    exit(1);
  }

  printf("输入要发送的消息：");
  fgets(message.msg_text, MAX_TEXT_SIZE, stdin);
  message.msg_type = 1; // 设置消息类型为1

  // 发送消息
  if (msgsnd(msgid, &message, sizeof(message), 0) == -1)
  {
    perror("msgsnd");
    exit(1);
  }

  printf("消息发送成功！\n");

  return 0;
}
