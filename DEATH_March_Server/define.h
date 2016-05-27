#ifndef DEFINE_H
#define DEFINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

//
#include <opencv2/opencv.hpp>
#include <iostream>
#include<sys/time.h>


//watnabe
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include<iterator>
#include <fstream>

using namespace std;
using namespace cv;



void tranceport(int socket, char *binary,int size);
void jusin(int socket, char *path, long fsize, char *binary);
char *getFileName(char *path);
unsigned long filesize(char *path);





//



#define BUF_SIZE_256		256
#define PORT1				50000
#define PORT2				50001
#define PORT3                           50002

#define FILE_NUM 50


////labeling///


//color for labeling
//http://www.color-sample.com
static Vec3b Winered = Vec3b(63,39,128);//winered
static Vec3b Bleumarine = Vec3b(113,63,0);//Bleumarine
static Vec3b Red = Vec3b(0,0,255);//Red
static Vec3b Boubainvillea = Vec3b(122,22,208);//Boubainvillea
static Vec3b Blueviolet = Vec3b(226,43,138);//Blueviolet
static Vec3b Orange = Vec3b(15,129,239);//Orange
static Vec3b Forestgreen = Vec3b(34,139,34);//Forestgreen
static Vec3b Gold = Vec3b(0,215,255);//Gold
static Vec3b Marron = Vec3b(40,68,127);//Marron
static Vec3b Skin = Vec3b(183,216,248);//Skin
static Vec3b Blue = Vec3b(255,0,0);//Blue


//決め打ちしてるけど使ってない
#define Y_SIZE 1080
#define X_SIZE 1920

//ラベルの最大数
#define MAX_LABEL 200

//LUTの最大数
#define LUT_NUM 40000


int label_convert(int num, int *label);//ラベルの色付けの
void paint(Mat_<Vec3b> input,int *labelnum);//色付け

//ラベリング
void label_up(Mat_<Vec3b> input, int *labelnum,int *LUT, int &count_min_label, int x, int y);
void label_left(Mat_<Vec3b> input, int *labelnum,int *LUT, int &count_min_label, int x, int y);
void label_right(Mat_<Vec3b> input, int *labelnum,int *LUT, int &count_min_label,int x,int y);
void label_nomal(Mat_<Vec3b> input, int *labelnum,int *LUT,int &count_min_label, int x,int y);

void first_path(Mat_<Vec3b> input, int *labelnum,int *LUT);

void labelling(Mat_<Vec3b> input, int *labelnum);

void labeling_main(Mat_<Vec3b> input,char filename[]);

#endif
