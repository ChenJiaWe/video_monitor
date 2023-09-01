#include <stdio.h>
#include "cgic.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define QUEUE_MSG_LEN 32

#define MAX_TEXT_SIZE 100

struct msg
{
	long msgtype;
	long type;
	char text[QUEUE_MSG_LEN];
};

int cgiMain()
{
	char store_value[10];
	char beep_value[10];
	int msgid;
	int ret;
	struct msg msg_buf;
	char value;
	int fd;
	key_t key;

	if (cgiFormString("store", store_value, sizeof(store_value)) != cgiFormSuccess)
	{
		fprintf(cgiOut, "Failed to get store field.\n");
		return -1;
	};
	if (cgiFormString("beep", beep_value, sizeof(beep_value)) != cgiFormSuccess)
	{
		fprintf(cgiOut, "Failed to get beep_value field.\n");
		return -1;
	};

	if ((key = ftok("/tmp", 'g')) < 0)
	{
		perror("ftok");
		exit(1);
	};

	if ((msgid = msgget(key, 0666)) < 0)
	{
		perror("msgget");
		exit(1);
	};

	msg_buf.msgtype = 1L;
	msg_buf.text[0] = beep_value[0];
	msg_buf.type = 2L;

	ret = msgsnd(msgid, &msg_buf, sizeof(msg_buf), 0);

	cgiHeaderContentType("text/html\n\n");
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>My CGI</TITLE></HEAD>\n");
	fprintf(cgiOut, "<BODY>");

	fprintf(cgiOut, "<H2>send sucess</H2>");
	fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"1;url=../contrl.html\">");
	fprintf(cgiOut, "</BODY>\n");
	fprintf(cgiOut, "</HTML>\n");

	return 0;
}
