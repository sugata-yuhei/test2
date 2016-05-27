/**************************************************************************************
 * ファイル送信サーバ
 *
 * 説明：
 * 　クライアントから指定されたファイルをクライアントに送信する。
 * 　TCP/IPの機能のみを使用している。
 *
 * 作成日：2011/10/23
 **************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <pthread.h>

#ifndef NTHREADS
#define NTHREADS 4 /* default 4 threads */
#endif

#define BUF_SIZE_256		256
#define PORT1				50000
#define PORT2				50001
#define PORT3                           50002

int pid;

void tranceport(int socket, char *path);
void jusin(int socket, long fsize, char *text);
char *getFileName(char *path);
unsigned long filesize(char *path);

typedef struct thread_args {
  int my_id;
  char* text;
  long my_start;
  long my_end;
  long my_kiyoshi;
} thread_args;


void *
thread_func(void *argp)
{
  thread_args *my_args = (thread_args *)argp;
  char *t=my_args->text;

  long m_start=my_args->my_start;
  long m_end=my_args->my_end;

  int z_cnt=0;
  long i;

  i = m_start;
     
  while(i <= m_end){
    if(t[i] == 'z'){
      z_cnt++;
      i+=3;
    }
    else if((t[i] == 'd') && (z_cnt >= 4)){
      my_args->my_kiyoshi++;
      i+=4;
      z_cnt = 0;
    }
    else{
      z_cnt = 0;
      i+=4;
    }
  } 
  return NULL;
}



int main(int argc, char **argv)
{
	int s1;			// 接続受付用ソケット(メッセージ送受信用ソケット生成担当)
	int s2;			// 接続受付用ソケット(データ送受信用のソケット生成担当)
	int ms;			// メッセージ送受信用ソケット
	int ds;			// データ送受信用ソケット
	int s3;
	struct sockaddr_in server;		// サーバ設定(メッセージ送受信用)
	struct sockaddr_in dserver;		// サーバ設定(データ送受信用サーバ設定)
	struct sockaddr_in dserver1;
	struct sockaddr_in client;		// クライアント設定
	int len;

	unsigned long size;
	char *text;
	long kiyoshi=0;
	long my_start[NTHREADS];
	long my_end[NTHREADS];
	long range,i,j;

	pthread_t tids[NTHREADS];
	thread_args args[NTHREADS];


	/*
	 * 接続受付用ソケットの生成
	 */
	// 接続受付用ソケット生成
	if ((s1 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		fprintf(stderr, "ソケット作成に失敗しました。\n");
		exit(1);
	}

	// 接続受付用ソケット生成
	if ((s2 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		fprintf(stderr, "ソケット作成に失敗しました。\n");
		exit(1);
	}
	// ファイル受信用のソケットを作成
       	if ((s3 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
       		fprintf(stderr, "ソケットの作成に失敗しました。\n");
       		exit(1);
	}
	

	/*
	 * サーバ情報の設定
	 */
	// メッセージ送受信用設定
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = PORT1;

	// データ送受信用設定
	memset(&dserver, 0, sizeof(dserver));
	dserver.sin_family = AF_INET;
	dserver.sin_addr.s_addr = htonl(INADDR_ANY);
	dserver.sin_port = PORT2;

	//データ受信用
	memset(&dserver1, 0, sizeof(dserver1));
	dserver1.sin_family = AF_INET;
	dserver1.sin_addr.s_addr = htonl(INADDR_ANY);
	dserver1.sin_port = PORT3;
	// サーバ情報の設定
	if (bind(s1, (struct sockaddr *) &server, sizeof(server)) < 0)
	{
		fprintf(stderr, "バインドに失敗しました。\n");
		exit(1);
	}

	// サーバ情報の設定
	if (bind(s2, (struct sockaddr *) &dserver, sizeof(dserver)) < 0)
	{
		fprintf(stderr, "バインドに失敗しました。\n");
		exit(1);
	}

        // サーバ情報の設定
	if (bind(s3, (struct sockaddr *) &dserver1, sizeof(dserver1)) < 0)
	{
		fprintf(stderr, "バインドに失敗しました。\n");
		exit(1);
	}


	/*
	 * 接続の受付開始
	 */
	if (listen(s1, 5) < 0)
	{
		fprintf(stderr, "コネクション受付開始\n");
		exit(1);
	}


	if (listen(s2, 5) < 0)
	{
		fprintf(stderr, "コネクション受付開始\n");
		exit(1);
	}
	if(listen(s3,5) < 0)
	{
		fprintf(stderr, "コネクション受付開始\n");
		exit(1);
	}

	/*
	 * 通信処理
	 */
		// メッセージ送受信用接続の受付
		len = sizeof(client);
		if ((ms = accept(s1, (struct sockaddr *) &client, &len)) < 0)
		{
			fprintf(stderr, "クライアントとの接続に失敗しました。\n");
			exit(1);
		}

			printf("通信を開始します。\n");
			printf("client address ... %s\n", inet_ntoa(client.sin_addr));

			for (;;)
			{
				int recvByte;
				char buf[BUF_SIZE_256];

				recv(ms,&size,sizeof(long),0);
				printf("受信するファイルのサイズ(サーバー):%d\n",size);

				/* メモリ領域の確保 */
				text = malloc(size * sizeof(char) + 1);

				len = sizeof(client);
				if ((ds = accept(s3, (struct sockaddr *) &client, &len)) < 0)
				{
					fprintf(stderr, "error\n");
					exit(1);
				}


				jusin(ds,size,text);

				range = (size + NTHREADS-1) / NTHREADS;
				my_start[0]=0;
				my_end[3]=size;
  
				i=0;
				j=0;
				while(i<NTHREADS-1){
				  j+=range;
				  while(text[j]!='d'){
				    j++;
				    if(j > size){
				      printf("info: cannot make threads\n");
				      i=NTHREADS-2;
				      break;
				    }
				  }
				  my_start[i+1]=j+4;
				  my_end[i]=j-1;
				  i++;
				}

				for (i = 0; i < NTHREADS; i++) {
				  args[i].my_id = i;
				  args[i].text = text;
				  args[i].my_start=my_start[i];
				  args[i].my_end=my_end[i];
				  args[i].my_kiyoshi = 0;
				}
				/* create threads */
				for (i = 0; i < NTHREADS; i++) {
				  pthread_create(&tids[i], NULL, thread_func, &args[i]);
				}
  
				/* wait for all threads termination */
				for (i = 0; i < NTHREADS; i++) {
				  pthread_join(tids[i], NULL);
				}
  
				for (i = 0; i < NTHREADS; i++) {
				  kiyoshi+=args[i].my_kiyoshi;
				} 
		
				// データ送受信用接続の受付
				len = sizeof(client);
				if ((ds = accept(s2, (struct sockaddr *) &client, &len)) < 0)
				{
					fprintf(stderr, "error\n");
					exit(1);
				}

				send(ds,&kiyoshi,sizeof(long),0);

				printf("転送完了\n");
				close(ds);
                       	}

end:
	close(s1);
	close(s2);
	close(s3);

	return 0;
}


/**************************************************************************************
 * ファイルパスからファイル名を抽出
 *
 * 引数：path ファイルパス
 **************************************************************************************/
char *getFileName(char *path)
{
	char *ptoken;
	char *pret;

	ptoken = strtok(path, "/");
	while ((pret = strtok(NULL, "/")) != NULL)
	{
		ptoken = pret;
	}

	return ptoken;
}


/**************************************************************************************
 * ファイル受信処理
 *
 * textに受信したテキストデータを格納
 * 
 **************************************************************************************/
void jusin(int socket, long fsize, char *text)
{
	long size;
	long recv_size;

	recv_size = 0;
	size = 0;

	for (;;)
	{
		recv_size = recv(socket, &text[size], 4096, 0);
		size+=recv_size;
		if (size >= fsize)
		{
			break;
		}


	}
}

/**************************************************************************************
 * ファイル送信処理
 *
 * 引数：socket 送信用ソケット
 *       path 送信ファイルのパス
 **************************************************************************************/
void tranceport(int socket, char *path)
{
	FILE *fp;
	int size;
	char buf[4096];
	int send_size=0;
	long fsize;

	// ファイルオープン
	fp = fopen(path, "rb");
	if (fp == NULL)
	{
		fprintf(stderr, "%s not found.",path);
		exit(1);
	}
	fsize=filesize(path);

	// ファイル送信
	for (;;)
	{
		size = fread(buf, 1, 4096, fp);
		send_size+=size;
		send(socket,buf,size,0);

		if (send_size >= fsize)
		{
			break;
		}

	}

	// ファイルクローズ
	fclose(fp);
}

unsigned long filesize(char *name)
{
  FILE *fp;
  unsigned long size;

  fp=fopen(name,"rb");
  if(fp == NULL){
    printf("error : %s cannot open file\n",name);
  }
  fseek(fp,0,SEEK_END);
  size=ftell(fp);
  fseek(fp,0,SEEK_SET);
  fclose(fp);
  return size;
}


long found_kiyoshi(char *text, long size)
{



}
