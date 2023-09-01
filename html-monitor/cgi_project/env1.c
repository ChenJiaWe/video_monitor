#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include <sys/sem.h>
#include <unistd.h>
#include "cgic.h"
#include "env1.h"

struct shm_addr
{
  int shm_status;
  struct env_info_client_addr sm_all_env_info;
};

int cgiMain()
{
  key_t key;
  int shmid, semid;
  struct shm_addr *shm_buf;

  if ((key = ftok("/tmp", 'i')) < 0)
  {
    perror("ftok");
    exit(1);
  };
  printf("key=%x\n", key);
  if ((semid = semget(key, 1, 0666)) < 0)
  {
    perror("semget");
    exit(1);
  };
  if ((shmid = shmget(key, 32, 0666)) < 0)
  {
    perror("semget");
    exit(1);
  };

  if ((shm_buf = (struct shm_addr *)shmat(shmid, NULL, 0)) == (void *)-1)
  {
    perror("shmat");
    exit(1);
  };
  sem_p(semid);
  cgiHeaderContentType("text/html");
  fprintf(cgiOut, "<head><meta http-equiv=\"refresh\" content=\"1\"><style><!--body{ line-height:50% }--></style> </head>");
  fprintf(cgiOut, "<HTML>\n");
  fprintf(cgiOut, "<BODY bgcolor=\"#666666\">\n");
  // fprintf(cgiOut, "<h1><font color=\"#FF0000\">HOME_ID #%d:</font></H2>\n ", shm_buf->shm_status);
  if (shm_buf->shm_status == 1)
  {
    fprintf(cgiOut, "<script>function show(){var date =new Date(); var now = \"\"; now = date.getFullYear()+\"年\"; now = now + (date.getMonth()+1)+\"月\"; \ now = now + date.getDate()+\"日\"; now = now + date.getHours()+\"时\"; now = now + date.getMinutes()+\"分\";now = now + date.getSeconds()+\"秒\"; document.getElementById(\"nowDiv\").innerHTML = now; setTimeout(\"show()\",1000);} </script> \n ");
    fprintf(cgiOut, "<h2><font face=\"Broadway\"><font color=\"#00FAF0\">Home1 Real-time Environment Info:</font></font></H2>\n ");
    fprintf(cgiOut, "<h2 align=center><font color=\"#cc0033\"><body onload=\"show()\"> <div id=\"nowDiv\"></div></font></h2> \n ");

    fprintf(cgiOut, "<h4>数据显示部分</H4>\n ");
    fprintf(cgiOut, "<h4>adasdasdasd:\t%d</H4>\n ", key);
    fprintf(cgiOut, "<h4>adasdasdasd:\t%d</H4>\n ", shmid);
    fprintf(cgiOut, "<h4>IR:\t%d</H4>\n ", shm_buf->sm_all_env_info.monitor_no[shm_buf->shm_status].ap3216c_info.ir);
    fprintf(cgiOut, "<h4>ALS:\t%d</H4>\n ", shm_buf->sm_all_env_info.monitor_no[shm_buf->shm_status].ap3216c_info.als);
    fprintf(cgiOut, "<h4>PS:\t%d</H4>\n ", shm_buf->sm_all_env_info.monitor_no[shm_buf->shm_status].ap3216c_info.ps);

    fprintf(cgiOut, "<h4>STM32数据显示部分</H4>\n ");
    fprintf(cgiOut, "<h4>......</H4>\n ");
  }
  else
  {
    fprintf(cgiOut, "<h2><font face=\"Broadway\"><font color=\"#FFFAF0\">Close!</font></font></H2>\n ");
  }
  //	fprintf(cgiOut, "<h3>:</H3>\n ");
  fprintf(cgiOut, "</BODY></HTML>\n");
  sem_v(semid);

  return 0;
};