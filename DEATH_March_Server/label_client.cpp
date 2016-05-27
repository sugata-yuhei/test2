/**************************************************************************************
 * ファイル受信クライアント
 *
 * 説明：
 * 　サーバに保存されているファイルを指定し受信する。
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
#include <dirent.h>

#define BUF_SIZE_256		256

char *getFileName(char *path);
void jusin(int socket, char *path,long fsize);
void tranceport(int socket, char *path);
unsigned long filesize(char *path);

int main(int argc, char **argv)
{
        int s, ds,s3;
	struct sockaddr_in server;
	struct in_addr dst_ip;
	unsigned short port  = 50000;
	unsigned short port2 = 50001;
	unsigned short port3 = 50002;
	unsigned long size;
	DIR *dir;
	struct dirent *dp;
      

	char path[64] = "./label_input/";
	char fname[256];
	//char buf[BUF_SIZE_256]="lena.jpg";

	dst_ip.s_addr = inet_addr(argv[1]);

	printf("アドレスをバイト列に変換\n");
	printf("addres to byte ... %d\n", dst_ip.s_addr);
	printf("バイト列をＩＰｖ４形式の文字列に変換\n");
	printf("byte to address ... %s\n", inet_ntoa(dst_ip));

	// メッセージ送受信用ソケット生成
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		fprintf(stderr, "ソケットの作成に失敗しました。\n");
		exit(1);
	}

	// サーバ接続
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = dst_ip.s_addr; 
	server.sin_port = port;

	if (connect(s, (struct sockaddr *) &server, sizeof(server)) < 0)
	{
		fprintf(stderr, "サーバへの接続に失敗しました。\n");
		exit(1);
	}

	printf("connect ... %s\n", inet_ntoa(server.sin_addr));

	dir = opendir(path);
	if (dir == NULL){
	  printf("error: path cannot open\n");
	  exit(-1);
	}

	for (;;) // face detection
	{
	  char buf[BUF_SIZE_256]={0};

		//	printf("> ");
		/* 送信するファイル名を取得 */
		//	fgets(buf, BUF_SIZE_256, stdin);

         	//size=filesize(getFileName(buf));
		//printf("%d",size);
		
		dp = readdir(dir);
		//	       	printf("d_name :%s\n",dp->d_name);
		
		if( dp == NULL){
		  printf("found null\n");
		  break;
		}

		if( (strcmp(".",dp->d_name)==0) || ((strcmp("..",dp->d_name)==0))){
		  printf("found . or ..\n");
		  continue;
		}

     	
		strcpy(buf,dp->d_name);
		strcpy(fname,path);
		strcat(fname,buf);

		printf("file name :%s\n",buf);
		
		/* ファイル名を送信 */
		send(s, buf, strlen(buf), 0);
				
      		//buf[strlen(buf)-1] = '\0';
	
		if (strcmp("exit", buf) == 0)
		{
			printf("サーバとの接続を切断します。\n");
			close(s);
			break;
		}

                size=filesize(fname);
                printf("%d",size);
		send(s,&size,sizeof(long),0);


		server.sin_port=port3;
		//ファイル送信様のソケットを作成
                if ((s3 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			fprintf(stderr, "ソケットの作成に失敗しました。\n");
			exit(1);
		}
                if (connect(s3, (struct sockaddr *) &server, sizeof(server)) < 0)
		{
			fprintf(stderr, "データa通信用のソケット生成に失敗しました。\n");
			exit(1);
		}

                tranceport(s3,fname);

		// ファイル受信用のソケットを作成
		if ((ds = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			fprintf(stderr, "ソケットの作成に失敗しました。\n");
			exit(1);
		}
	
		// サーバへファイル受信用(ポート番号5001)で接続
		server.sin_port = port2;
		if (connect(ds, (struct sockaddr *) &server, sizeof(server)) < 0)
		{
			fprintf(stderr, "データb通信用のソケット生成に失敗しました。\n");
			exit(1);
		}
		recv(ds,&size,sizeof(long),0);
		printf("受信するファイルサイズStoC:%d\n",size);
		
		// データ受信
		jusin(ds, getFileName(buf),size);

		printf("ファイル受信が完了しました。\n");

		close(ds);
	}
	closedir(dir);

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
 * 引数：socket 受信用ソケット
 *       name 受信ファイル名
 **************************************************************************************/
void jusin(int socket, char *name,long fsize)
{
	FILE *fp;
	int size;
	char buf[4096];
	long recv_size=0;
	char fname[256]="./label_output/";

	// ファイルオープン
	
	strcat(fname,name);

	fp = fopen(fname, "wb");
	if (fp == NULL)
	{
		fprintf(stderr, "%s could not open.", name);
		exit(1);
	}

	// ファイル受信
	for (;;)
	{
		size = recv(socket, buf, 4096, 0);
		recv_size+=size;
		fwrite(buf,1,size,fp);

		if (recv_size >= fsize)
		{
			break;
		}

	}

	// バッファ内容をフラッシュ
	fflush(fp);

	// ファイルクローズ
	fclose(fp);
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

	fsize = filesize(path);
	
	// ファイル送信
	//	while(send_size < fsize)
       	for (;;)
	{
		size = fread(buf, 1, 4096, fp);
		send_size+=size;
		send(socket, buf, size, 0);
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
