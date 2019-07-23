#include <stdio.h>
#include <sys/shm.h> //内存共享API所需的库
#include <unistd.h> //进程开辟所需的库
#include <string.h>
#include <pthread.h> //多线程编程所需的库
#include <sys/socket.h> //套接字所需的库
#include <netinet/in.h>
#include <stdlib.h>

#define TEXT_SIZE 500
int shmid; //共享内存标识
int sdd;
int ret = 0;
//函数声明
int create_shm(void); //创建共享内存的函数
void child(int sd, int id);//fork出的进程所执行的函数
void *pthread_func(void *arg);

struct text_buf
{
    int written; // 作为一个标志，1：表示不可写，0：表示可写
    char text[TEXT_SIZE]; // 记录写入 和 读取 的文本
};

int main(int argc, char **argv) {

    int listenfd; //套接字描述符
    int connfd; //新连接的套接字描述符
	int num = 0; //记录连接的客户端数量
    struct sockaddr_in servaddr; //socket地址变量

    listenfd = socket(PF_INET, SOCK_STREAM, 0); //创建套接字
    bzero(&servaddr, sizeof(servaddr)); //地址的初始化
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //本地ip
    servaddr.sin_port = htons(8080); //端口
	bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)); //将地址与套接字绑定
    listen(listenfd, 10); //监听模式，最大连接数
	if((shmid = create_shm()) == -1) {
		printf("创建共享内存时错误，程序结束！\n");
		exit(1);
	}
	
	while(1) {
		connfd = accept(listenfd, (struct sockaddr *)NULL, NULL); //监听客户端的连接，返回与客户端连接的套接字描述符。注意此时会阻塞
		num++; //有连接时num自增。
		/*while(1){
		    int n;
		    char recvline[1024];
		
		    while((n=read(connfd, recvline, 1024)) > 0)
		    {
		        recvline[n] = 0;
		        printf("%s\n", recvline);
		    }
		}*/
		int pid; //保存fork()的返回值用于区分父子进程
		pid = fork();
		if(pid == 0) { //子进程
			close(listenfd);	
			child(connfd, num);
		}else if(pid > 0){ //父进程，close掉新连接套接字，继续监听等待新连接的到来。
			close(connfd);
		}else { //fork失败，结束程序。
			break;
		}
	}
	close(listenfd);
    return 1;
}

void child(int sd, int id) {
	//变量定义
	struct text_buf *buffer = (struct text_buf *)shmat(shmid, NULL, 0); //子进程内存映射共享内存
	char client_buf[TEXT_SIZE]; //缓存客户端发来的数据
	char server_buf[TEXT_SIZE]; //缓存共享内存中的数据
	int cangetbuf = 1;
	int n;
	pthread_t pth;
	int **retval;

	//初始化
	sdd = sd;	
	buffer->written = 0;	
	/**(buffer->text)	= 'a';
	*(buffer->text + 1) = 0;*/
	
	//pthread_create(&pth, NULL, pthread_func, client_buf);
	//	printf("fafdaf\n");
	while(1) {		
		//printf("buffer->text is %s\n", buffer->text);
		//printf("buffer->text is %s    the ret is %d\n", buffer->text, ret)    ;

		if(cangetbuf) {
			pthread_create(&pth, NULL, pthread_func, client_buf);
			//	pthread_join(pth, (void **)retval);
			cangetbuf = 0;
			/*if((n = read(sd, client_buf, TEXT_SIZE)) > 0) {
				client_buf[n] = 0;
				cangetbuf = 0;
			}*/
		}
		//printf("ID:%d   buffer->text is %s    the ret is %d\n", id, buffer->text, ret);
		if( (ret == 1) && (cangetbuf == 0) && (buffer->written == 0)) {
			//pthread_join(pth, (void **)retval);
			buffer->written = 1;
			strcpy(buffer->text, client_buf);
			cangetbuf = 1;
			ret = 0;
			buffer->written = 0;
		}
  		if(strcmp(server_buf, buffer->text) != 0) {
			strcpy(server_buf, buffer->text);
			write(sd, server_buf, strlen(server_buf));	
		}
	}
	

	/*if(id == 1) {
		strcpy(buffer->text, "hhhhhhhhhhhhhhhhh");
		shmdt((void *)buffer);
	}
	if(id == 2) {
		write(sd, buffer->text, strlen(buffer->text));
		shmdt((void *)buffer);
	}*/


	/*while(1){
        int n;
        char recvline[1024];
    
        while((n=read(sd, recvline, 1024)) > 0)
        {
            recvline[n] = 0;
            printf("%d sent %s \n", id, recvline);
        }
    }*/
}

int create_shm(void) {
	return shmget(0, sizeof(struct text_buf), 0666 | IPC_PRIVATE);	
}

void *pthread_func(void *arg) {
	int n;
	pthread_detach(pthread_self());
	//printf("into the pthread, read to get Text\n");
	n = read(sdd, (char *)arg, TEXT_SIZE);
	*((char *)arg + n) = 0;
	//printf("get the Text is %s\n", (char *)arg);
	ret = 1;
	pthread_exit(NULL);
}
