#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
void server(int sd, int id);
int main(int argc, char **argv)
{
    int listenfd;
    int connfd;
	int num = 0;
    struct sockaddr_in servaddr;

    listenfd = socket(PF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(8080);

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, 10);
	while(1) {
		connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);
		num++;
		/*while(1){
		    int n;
		    char recvline[1024];
		
		    while((n=read(connfd, recvline, 1024)) > 0)
		    {
		        recvline[n] = 0;
		        printf("%s\n", recvline);
		    }
		}*/
		int pid;
		pid = fork();
		if(pid == 0) {
			close(listenfd);	
			server(connfd, num);
		}else if(pid > 0){
			close(connfd);
		}else { 
			break;
		}
	}
	close(listenfd);
    return 1;
}

void server(int sd, int id) {
    while(1){
        int n;
        char recvline[1024];
    
        while((n=read(sd, recvline, 1024)) > 0)
        {
            recvline[n] = 0;
            printf("%d sent %s \n", id, recvline);
        }
    }   	
}
