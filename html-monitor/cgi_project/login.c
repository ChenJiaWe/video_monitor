#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// #include "sqlite3.h"
#include "cgic.h"

#define N 64

char name[N] = {0};
char pass[N] = {0};

char *getdata(char *req_method)
{
	char *content_length_str = getenv("CONTENT_LENGTH");
	int content_length = 0;
	char *post_data;
	int bytes_read;
	if (content_length_str != NULL)
	{
		content_length = atoi(content_length_str);
	};
	if (content_length <= 0)
	{
		printf("Content-Type: text/html\n\n");
		printf("<h1>Invalid Content Length</h1>");
		return NULL;
	}

	if (req_method != NULL && strcmp(req_method, "GET") == 0)
	{
		printf("Content-Type: text/html\n\n");
		printf("<h1>GET Request</h1>");
		return NULL;
	};
	if (req_method != NULL && strcmp(req_method, "POST") == 0)
	{
		post_data = (char *)malloc(content_length + 1);
		if (post_data == NULL)
		{
			printf("Content-Type: text/html\n\n");
			printf("<h1>Memory Allocation Failed</h1>");
			return NULL;
		};
		bytes_read = fread(post_data, 1, content_length, stdin);
		if (bytes_read != content_length)
		{
			printf("Content-Type: text/html\n\n");
			printf("<h1>Read Error</h1>");
			free(post_data);
			return NULL;
		};
		post_data[bytes_read] = '\0';

		return post_data;
	};
	return NULL;
};

void input_decode(char *input)
{
	char *dest = input;
	int i = 9, j = 0;
	while (dest[i] != '&' && dest[i] != '\0')
	{
		name[j] = dest[i];
		j++;
		i++;
	};
	if (dest[i] == '&')
	{
		name[j] = '\0';
	};
	i = 24;
	j = 0;
	while (dest[i] != '\0')
	{
		pass[j] = dest[i];
		i++;
		j++;
	};
	pass[j] = '\0';

	if ((strcmp(name, "Romeo") == 0) && (strcmp(pass, "123") == 0))
	{
		printf("<script language='javascript'>document.location = 'http://192.168.101.103/choose.html'</script>"); // �Զ���ת�����ҳ��
	}
	else
	{
		printf("输入的用户不存在");
		// exit(-1);
	}
};

int main()
{
	char *input;
	char *req_method;
	printf("Content-Type: text/html\n\n");

	req_method = getenv("REQUEST_METHOD");

	input = getdata(req_method);

	input_decode(input);

	return 0;
}
