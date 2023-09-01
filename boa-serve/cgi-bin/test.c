#include <stdio.h>
int main()
{
  printf("Content-type:text/html\n\n"); // 这句一定要加上
  printf("<html><body>");
  printf("<font style=\"color:red; font-size:30px;\">Hello, CGI!</font><br/>");
  printf("<a href=\"/index.html\">return index.html</a>");
  printf("</body></html>");
  return 0;
}