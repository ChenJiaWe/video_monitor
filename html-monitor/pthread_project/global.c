#include "global.h"

// 信号量操作函数
void sem_p(int semid)
{
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_op = -1;
  sb.sem_flg = SEM_UNDO;
  semop(semid, &sb, 1);
}

void sem_v(int semid)
{
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_op = 1;
  sb.sem_flg = SEM_UNDO;
  semop(semid, &sb, 1);
}
