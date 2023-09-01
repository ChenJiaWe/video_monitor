#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#define SHM_SIZE 1024

typedef struct
{
  char data[SHM_SIZE];
} SharedData;

int main()
{
  int shm_fd;
  sem_t *mutex;

  // 打开现有的共享内存
  shm_fd = shm_open("/my_shared_memoryc", O_CREAT | O_RDWR, 0666);
  if (shm_fd == -1)
  {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }

  // 将共享内存映射到进程地址空间
  SharedData *shared_data = (SharedData *)mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (shared_data == MAP_FAILED)
  {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  // 打开现有的互斥信号量
  mutex = sem_open("/my_mutex", 0);
  if (mutex == SEM_FAILED)
  {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  while (1)
  {
    sem_wait(mutex);

    // 读取共享内存中的数据
    printf("Data read from shared memory: %s", shared_data->data);

    sem_post(mutex);

    sleep(1); // 为了演示目的，读取数据稍微慢一点
  }

  // 清理资源
  munmap(shared_data, sizeof(SharedData));

  return 0;
}
