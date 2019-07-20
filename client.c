#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(PF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	while(1){
    	char sendline[100];
    	scanf("%s", sendline);
		//sprintf(sendline, "Hello, world!");
		printf("%s\n", sendline);
    	write(sockfd, sendline, strlen(sendline));
	}
    close(sockfd);

    return 1;
}

