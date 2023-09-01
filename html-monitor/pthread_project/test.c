#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define SHM_SIZE 128

union semun
{
  int val;
  struct semid_ds *buf;
  unsigned short *arry;
};

static int sem_id = 0;
int real_i = 0;

static int set_semvalue();
static void del_semvalue();
static int semaphore_p();
static int semaphore_v();

int main(int argc, char *argv[])
{
  int shm_id = 0;
  char message = 'X';
  char *share;
  int i = 0;

  // 创建信号量
  sem_id = semget((key_t)1234, 1, 0666 | IPC_CREAT);

  // 1.创建共享内存，返回一个标识
  shm_id = shmget((key_t)1234, SHM_SIZE, IPC_CREAT | 0666);
  if (shm_id == -1)
  {
    perror("shmget()");
  }
  // 2.将创建的共享内存与虚拟地址进行关联
  share = shmat(shm_id, NULL, 0);

  if (argc > 1)
  {
    // 程序第一次被调用，初始化信号量
    if (!set_semvalue())
    {
      fprintf(stderr, "Failed to initialize semaphore\n");
      exit(EXIT_FAILURE);
    }
    message = argv[1][0];
    sleep(2);
  }
  for (i = 0; i < 10; ++i)
  {
    // 进入临界区
    if (!semaphore_p())
      exit(EXIT_FAILURE);
    printf("%c", message);
    fflush(stdout);
    sprintf(share + strlen(share), "%c", message);
    real_i += 1;
    sleep(rand() % 3);
    sprintf(share + strlen(share), "%c", message);
    real_i += 1;
    // 离开临界区，休眠随机时间后继续循环
    printf("%c", message);
    fflush(stdout);
    if (!semaphore_v())
      exit(EXIT_FAILURE);
    sleep(rand() % 2);
  }

  sleep(10);
  printf("\n %s - finished\n", share);

  if (argc > 1)
  {
    // 如果程序是第一次被调用，则在退出前删除信号量
    sleep(3);
    del_semvalue();
  }

  // 3.把共享内存从当前进程中分离
  shmdt(share);
  // 4.删除共享内存
  shmctl(shm_id, IPC_RMID, 0);

  return 0;
}

static int set_semvalue()
{
  // 用于初始化信号量，在使用信号量前必须这样做
  union semun sem_union;

  sem_union.val = 1;
  if (semctl(sem_id, 0, SETVAL, sem_union) == -1)
    return 0;
  return 1;
}

static void del_semvalue()
{
  // 删除信号量
  union semun sem_union;

  if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
    fprintf(stderr, "Failed to delete semaphore\n");
}

static int semaphore_p()
{
  // 对信号量做减1操作，即等待P（sv）
  struct sembuf sem_b;
  sem_b.sem_num = 0;
  sem_b.sem_op = -1; // P()
  sem_b.sem_flg = SEM_UNDO;
  if (semop(sem_id, &sem_b, 1) == -1)
  {
    fprintf(stderr, "semaphore_p failed\n");
    return 0;
  }
  return 1;
}

static int semaphore_v()
{
  // 这是一个释放操作，它使信号量变为可用，即发送信号V（sv）
  struct sembuf sem_b;
  sem_b.sem_num = 0;
  sem_b.sem_op = 1; // V()
  sem_b.sem_flg = SEM_UNDO;
  if (semop(sem_id, &sem_b, 1) == -1)
  {
    fprintf(stderr, "semaphore_v failed\n");
    return 0;
  }
  return 1;
}