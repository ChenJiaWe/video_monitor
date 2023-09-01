#include "global.h"

int msgid;
int shmid;
int semid;

extern pthread_mutex_t mutex_buzzer;
extern pthread_mutex_t mutex_client_request;
extern pthread_mutex_t mutex_refresh;

extern pthread_cond_t cond_buzzer;
extern pthread_cond_t cond_client_request;
extern pthread_cond_t cond_refresh;

extern pthread_cond_t cond_transfer;
extern pthread_mutex_t mutex_transfer;

key_t shm_key;
key_t key; // msg_key
key_t sem_key;

char beep_cmd;

sem_t mutex;

struct msg msgbuf;

struct shm_addr
{
  char shm_status;
  struct env_info_client_addr sm_all_env_info;
};
struct shm_addr *shm_buf;

void *pthread_buzzer(void *arg)
{
  int fd;
  fd = open("/dev/my_beep", O_WRONLY);
  if (fd < 0)
  {
    perror("Error opening device file");
    exit(1);
  };
  while (1)
  {
    pthread_mutex_lock(&mutex_buzzer);
    pthread_cond_wait(&cond_buzzer, &mutex_buzzer);
    printf("pthread_buzzer\n");
    write(fd, &beep_cmd, sizeof(beep_cmd));
    pthread_mutex_unlock(&mutex_buzzer);
  };
};

void *pthread_client_request(void *arg)
{
  if ((key = ftok("/tmp", 'g')) < 0)
  {
    perror("ftok failed .\n");
    exit(-1);
  }

  msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
  if (msgid == -1)
  {
    if (errno == EEXIST)
    {
      msgid = msgget(key, 0777);
    }
    else
    {
      perror("fail to msgget");
      exit(1);
    }
  }
  printf("pthread_client_request\n");

  while (1)
  {
    memset(&msgbuf, 0, sizeof(msgbuf));
    printf("wait form client request...\n");
    msgrcv(msgid, &msgbuf, sizeof(msgbuf), 1L, 0);
    printf("Get %ldL msg\n", msgbuf.msgtype);
    printf("Get %ldL type\n", msgbuf.type);
    printf("text[0] = %c\n", msgbuf.text[0]);

    switch (msgbuf.type)
    {
    case 1L:
      printf("hello led\n");
      break;
    case 2L:
      pthread_mutex_lock(&mutex_buzzer);
      pthread_cond_signal(&cond_buzzer);
      printf("hello beep\n");
      beep_cmd = msgbuf.text[0];
      pthread_mutex_unlock(&mutex_buzzer);
      break;
    case 3L:
      printf("hello seg\n");
      break;
    case 4L:
      printf("hello fan\n");
      break;
    default:
      break;
    }
  };

  exit(0);
};

static int file_ap3216c_info_struct(struct env_info_client_addr *rt_status, int home_id, uint16_t data[3])
{
  int env_info_size = sizeof(struct env_info_client_addr);
  printf("env_info_size = %d.\n", env_info_size);

  rt_status->monitor_no[home_id].ap3216c_info.als = data[0];
  rt_status->monitor_no[home_id].ap3216c_info.ir = data[1];
  rt_status->monitor_no[home_id].ap3216c_info.ps = data[2];

  return 0;
};

void *pthread_transfer(void *arg)
{

  int fd;
  char *filename;
  uint16_t databuf[3];
  uint16_t ir, als, ps;
  int ret = 0;
  fd = open("/dev/my_ap3216c", O_RDWR);
  if (fd < 0)
  {
    fd = open("/dev/ap3216c", O_RDWR);
    if (fd < 0)
    {
      printf("can't open file %s\r\n", filename);
      exit(-1);
    }
  };
  while (1)
  {
    /* code */
    pthread_mutex_lock(&mutex_transfer);
    pthread_cond_wait(&cond_transfer, &mutex_transfer);
    printf("pthread_transfer\n");
    ret = read(fd, databuf, sizeof(databuf));
    if (ret == 0)
    {                   /* 数据读取成功 */
      ir = databuf[0];  /* ir传感器数据 */
      als = databuf[1]; /* als传感器数据 */
      ps = databuf[2];  /* ps传感器数据 */
      printf("ir = %d, als = %d, ps = %d\r\n", ir, als, ps);
      if (shm_buf->shm_status)
      {
        file_ap3216c_info_struct(&shm_buf->sm_all_env_info, shm_buf->shm_status, databuf);
      };
    }
    pthread_mutex_unlock(&mutex_transfer);
    sleep(1);
  };

  exit(0);
}

void *pthread_refresh(void *arg)
{
  // semaphore for access to resource limits
  if ((sem_key = ftok("/tmp", 'i')) < 0)
  {
    perror("ftok failed .\n");
    exit(-1);
  }
  semid = semget(sem_key, 1, IPC_CREAT | IPC_EXCL | 0666);
  if (semid == -1)
  {
    if (errno == EEXIST)
    {
      semid = semget(sem_key, 1, 0777);
    }
    else
    {
      perror("fail to semget");
      exit(1);
    }
  }
  else
  {
    semctl(semid, 0, SETVAL, 1);
  }

  if ((shm_key = ftok("/tmp", 'i')) < 0)
  {
    perror("ftok failed .\n");
    exit(-1);
  };
  shmid = shmget(shm_key, N, IPC_CREAT | IPC_EXCL | 0666);
  if (shmid == -1)
  {
    if (errno == EEXIST)
    {
      shmid = shmget(key, N, 0777);
    }
    else
    {
      perror("fail to shmget");
      exit(1);
    }
  };

  // share memap
  if ((shm_buf = (struct shm_addr *)shmat(shmid, NULL, 0)) == (void *)-1)
  {
    perror("fail to shmat");
    exit(1);
  }

  printf("pthread_refresh ......>>>>>>>\n");

  memset(shm_buf, 0, sizeof(struct shm_addr));
  while (1)
  {
    sem_p(semid);
    shm_buf->shm_status = 1;
    // file_env_info_struct(&shm_buf->sm_all_env_info, shm_buf->shm_status);
    pthread_cond_signal(&cond_transfer);
    sleep(1);
    sem_v(semid);
  };
};
// int file_env_info_struct(struct env_info_client_addr *rt_status, int home_id)
// {
//   int env_info_size = sizeof(struct env_info_client_addr);
//   printf("env_info_size = %d.\n", env_info_size);

//   rt_status->monitor_no[home_id].ap3216c_info.als = 16;
//   rt_status->monitor_no[home_id].ap3216c_info.ir = 12;
//   rt_status->monitor_no[home_id].ap3216c_info.ps = 14;

//   rt_status->monitor_no[home_id].info.adc = 9.0;
//   rt_status->monitor_no[home_id].info.gyrox = -14.0;
//   rt_status->monitor_no[home_id].info.gyroy = 20.0;
//   rt_status->monitor_no[home_id].info.gyroz = 40.0;
//   rt_status->monitor_no[home_id].info.aacx = 642.0;
//   rt_status->monitor_no[home_id].info.aacy = -34.0;
//   rt_status->monitor_no[home_id].info.aacz = 5002.0;
//   rt_status->monitor_no[home_id].info.reserved[0] = 0.01;
//   rt_status->monitor_no[home_id].info.reserved[1] = -0.01;

//   return 0;
// };