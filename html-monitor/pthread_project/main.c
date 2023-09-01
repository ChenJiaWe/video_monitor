#include "global.h"

pthread_t id_buzzer, id_client_request, id_transfer, id_refresh;

pthread_mutex_t mutex_buzzer, mutex_client_request, mutex_transfer, mutex_refresh;

pthread_cond_t cond_buzzer, cond_client_request, cond_transfer, cond_refresh;

extern int msgid;
extern int shmid;
extern int semid;

static void release_pthread_resource2(void *arg)
{

  printf("release_pthread_resource2\n");
  pthread_mutex_destroy(&mutex_buzzer);
  pthread_mutex_destroy(&mutex_client_request);
  pthread_mutex_destroy(&mutex_refresh);

  pthread_cond_destroy(&cond_buzzer);
  pthread_cond_destroy(&cond_client_request);
  pthread_cond_destroy(&cond_refresh);

  pthread_detach(id_client_request);
  pthread_detach(id_buzzer);
  pthread_detach(id_refresh);

  msgctl(msgid, IPC_RMID, NULL);
  shmctl(shmid, IPC_RMID, NULL);
  semctl(semid, 1, IPC_RMID, NULL);
};

int main(int argc, char const *argv[])
{
  signal(SIGINT, release_pthread_resource);
  // pthread_cleanup_push(release_pthread_resource2, NULL);
  pthread_mutex_init(&mutex_client_request, NULL);
  pthread_mutex_init(&mutex_buzzer, NULL);
  pthread_mutex_init(&mutex_refresh, NULL);
  pthread_mutex_init(&mutex_transfer, NULL);

  pthread_cond_init(&cond_client_request, NULL);
  pthread_cond_init(&cond_buzzer, NULL);
  pthread_cond_init(&cond_refresh, NULL);
  pthread_cond_init(&cond_transfer, NULL);

  pthread_create(&id_client_request, NULL, pthread_client_request, NULL);
  pthread_create(&id_buzzer, NULL, pthread_buzzer, NULL);
  pthread_create(&id_refresh, NULL, pthread_refresh, NULL);
  pthread_create(&id_transfer, NULL, pthread_transfer, NULL);

  pthread_join(id_client_request, NULL);
  pthread_join(id_buzzer, NULL);
  pthread_join(id_transfer, NULL);

  // pthread_cleanup_pop(1);

  return 0;
}

void release_pthread_resource(int sig)
{

  printf("release_pthread_resource\n");
  pthread_cancel(id_client_request); // 取消线程
  pthread_cancel(id_buzzer);
  pthread_cancel(id_refresh);
  pthread_cancel(id_transfer);

  pthread_mutex_destroy(&mutex_buzzer);
  pthread_mutex_destroy(&mutex_client_request);
  pthread_mutex_destroy(&mutex_refresh);
  pthread_mutex_destroy(&mutex_refresh);

  pthread_cond_destroy(&cond_buzzer);
  pthread_cond_destroy(&cond_client_request);
  pthread_cond_destroy(&cond_transfer);

  pthread_detach(id_client_request);
  pthread_detach(id_buzzer);
  pthread_detach(id_refresh);
  pthread_detach(id_transfer);

  msgctl(msgid, IPC_RMID, NULL);
  shmctl(shmid, IPC_RMID, NULL);
  semctl(semid, 1, IPC_RMID, NULL);
};
