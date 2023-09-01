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

  // 生成一个唯一的key，和发送端保持一致
  if ((key = ftok("/tmp", 'a')) == -1)
  {
    perror("ftok");
    exit(1);
  }

  // 获取消息队列
  if ((msgid = msgget(key, 0666 | IPC_CREAT)) == -1)
  {
    perror("msgget");
    exit(1);
  }

  // 接收消息
  if (msgrcv(msgid, &message, sizeof(message), 1, 0) == -1)
  {
    perror("msgrcv");
    exit(1);
  }

  printf("接收到的消息： %s\n", message.msg_text);

  // 删除消息队列
  if (msgctl(msgid, IPC_RMID, NULL) == -1)
  {
    perror("msgctl");
    exit(1);
  }

  return 0;
}
