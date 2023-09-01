#ifndef ENV1_H
#define ENV1_H

#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>

#define N 1024 // for share memory
#define QUEUE_MSG_LEN 32

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define MONITOR_NUM 1

struct msg
{
  long type;
  long msgtype;
  char text[QUEUE_MSG_LEN];
};


struct makeru_ap3216c_info
{
  uint16_t als;
  uint16_t ir;
  uint16_t ps;
};

struct makeru_info
{
  uint8_t head[3];
  uint8_t type;
  float adc;
  short gyrox;
  short gyroy;
  short gyroz;
  short aacx;
  short aacy;
  short aacz;
  uint32_t reserved[2];
};

struct makeru_env_data
{
  struct makeru_info info;
  struct makeru_ap3216c_info ap3216c_info;
  uint32_t reserved[2];
};


struct env_info_client_addr
{
  struct makeru_env_data monitor_no[MONITOR_NUM]; // ����  �ϼ�---�¼�
};

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

void *(pthread_buzzer)(void *arg);
void *(pthread_client_request)(void *arg);
void *pthread_refresh(void *arg);
void release_pthread_resource(int signo);
int file_env_info_struct(struct env_info_client_addr *rt_status, int home_id);
#endif
