#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#define TEXT_SIZE 500

void *pthread_func_write(void *arg);
void *pthread_func_read(void *arg);

int canget = 1;
int canread = 1;
int sd;

int main(int argc, char **argv) {
    int sockfd;
    struct sockaddr_in servaddr;
	pthread_t pth;
	pthread_t pth_read;
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
	sd = sockfd;

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	
	char sendline[TEXT_SIZE];
	char recvline[TEXT_SIZE];
	//scanf("%s", sendline);
	//write(sockfd, sendline, strlen(sendline));
	while(1) {
		if(canget) {
			pthread_create(&pth, NULL, pthread_func_write, sendline);
			canget = 0;
		}
		
		if(canread) {
			pthread_create(&pth_read, NULL, pthread_func_read, recvline);
			canread = 0;
		}
	}
	


	/*while(1){
          int n;
          char recvline[1024];
      
          while((n=read(sockfd, recvline, 1024)) > 0)
          {
              recvline[n] = 0;
              printf(" sent %s \n",recvline);
          }
      }*/

	/*while(1){
    	char sendline[100];
    	scanf("%s", sendline);
		//sprintf(sendline, "Hello, world!");
		printf("%s\n", sendline);
    	write(sockfd, sendline, strlen(sendline));
	}*/
    close(sockfd);

    return 1;
}
void *pthread_func_write(void *arg) {
	pthread_detach(pthread_self());
	scanf("%s", (char *)arg);
	write(sd, (char *)arg, strlen((char *)arg));
	canget = 1;
	pthread_exit(NULL);
}
void *pthread_func_read(void *arg) {
	pthread_detach(pthread_self());
	int n;
	n = read(sd, (char *)arg, TEXT_SIZE);
	*((char *)arg + n) = 0;
	printf("%s\n", (char *)arg);
	canread = 1;
	pthread_exit(NULL);
}
