#include "define.h"

int label_convert(int num, int *label){
  int i;
  for(i=0;i<MAX_LABEL;i++){
    if(label[i]==num) return i;
  }
  return -1;
}

void paint(Mat_<Vec3b> input,int *labelnum){
  int x,y;
  int i,j;
  int count=0;
  bool on;
  bool copy;
  bool debug;
  
  int label[MAX_LABEL];


  //init label
  for(i=0;i<MAX_LABEL;i++){
    label[i]=0;
  }


 for(y=0;y<input.rows;y++){
    for(x=0;x<input.cols;x++){//全範囲
	on = false;
	copy =false;
	if(labelnum[input.cols*y + x]!=0){//白いとこ 
	  for(i=0;i<count;i++){   
	    if(label[i]==labelnum[input.cols*y + x]) on =true;//もし登録されてたなら
	  }
	  if(on == true) continue;//次のxに
	  label[count]=labelnum[input.cols*y + x],count++; //ないなら開いているところに登録
	}
    }
  }



  for(y=0;y<input.rows;y++){
    for(x=0;x<input.cols;x++){
      if(labelnum[input.cols*y + x]!=0){
	switch(label_convert(labelnum[input.cols*y + x],label)){
	case 1:
	  input(y,x)=Winered;
	  break;
	case 2:
	  input(y,x)=Bleumarine;
	  break;
	case 3:
	  input(y,x)=Red;
	  break;
	case 4:
	  input(y,x)=Boubainvillea;
	  break;
	case 5:
	  input(y,x)=Blueviolet;
	  break;
	case 6:
	  input(y,x)=Orange;
	  break;
	case 7:
	  input(y,x)=Forestgreen;
	  break;
	case 8:
	  input(y,x)=Gold;
	  break;
	case 9:
	  input(y,x)=Marron;
	  break;
	case 10:
	  input(y,x)=Blue;
	  break;
	case 0:
	  input(y,x)=Skin;
	  break;
	default:
	  break;
	}
      }
    }
  }
}


void label_up(Mat_<Vec3b> input, int *labelnum,int *LUT, int &count_min_label, int x, int y){
  
int ref,rightup,leftup,up,left;
bool arround_white = false;
 int i;   
  //ラベルの番号をレジスタに保存
  ref = labelnum[input.cols*y + x];
  left=labelnum[input.cols*y + x-1];

  bool change;

  //update LUT
  for(;;){
    change = false;
    for(i=0;i<count_min_label;i++){
      if(LUT[i] != LUT[LUT[i]])  LUT[i] = LUT[LUT[i]] ,change=true;
    }
    if(change == false) break;
  }

  //まわりで白かつ最も小さいラベルをrefに入れる
  if((ref>left && left > 0) || (ref== 0 &&  left > 0))   ref=LUT[left],arround_white = true;
      
  if(arround_white == false){//回りに白がなかったら
       labelnum[input.cols*y + x] = count_min_label;
       count_min_label++;
  }
      
  if(arround_white == true){//回りに白があったら
      labelnum[input.cols*y + x] = ref;//最も小さい値を入れる
      //隣接するラベルの番号が違う場合LUTに保存
      if(left >0 && left != ref) LUT[left] = ref;
  }

}

void label_left(Mat_<Vec3b> input, int *labelnum,int *LUT, int &count_min_label, int x, int y){
int ref,rightup,leftup,up,left;
bool arround_white = false;
    int i;  
  //ラベルの番号をレジスタに保存
  ref = labelnum[input.cols*y + x];
  rightup=labelnum[input.cols*(y-1) + x+1];
  up=labelnum[input.cols*(y-1) + x];

  bool change;

  //update LUT
  for(;;){
    change = false;
    for(i=0;i<count_min_label;i++){
      if(LUT[i] != LUT[LUT[i]])  LUT[i] = LUT[LUT[i]] ,change=true;
    }
    if(change == false) break;
  }

  //まわりで白かつ最も小さいラベルをrefに入れる
  if((ref>rightup && rightup > 0)  || (ref== 0 &&  rightup > 0)) ref= LUT[rightup],arround_white = true;
  if((ref>up && up > 0 || (ref== 0) &&  up >0 ))      ref=LUT[up] ,arround_white = true;

  if(arround_white == false){//回りに白がなかったら
       labelnum[input.cols*y + x] = count_min_label;
       count_min_label++;
  }
      
  if(arround_white == true){//回りに白があったら
      labelnum[input.cols*y + x] = ref;//最も小さい値を入れる
      //隣接するラベルの番号が違う場合LUTに保存
      if(rightup >0 && rightup != ref) LUT[rightup] = ref;
      if(up >0 && up != ref) LUT[up] = ref;
  }
}


void label_right(Mat_<Vec3b> input, int *labelnum,int *LUT, int &count_min_label,int x,int y){
  
int ref,rightup,leftup,up,left;
bool arround_white = false;
    int i;  
  //ラベルの番号をレジスタに保存
  ref = labelnum[input.cols*y + x];
  up=labelnum[input.cols*(y-1) + x];
  leftup=labelnum[input.cols*(y-1) + x-1];
  left=labelnum[input.cols*y + x-1];

  bool change;

  //update LUT
  for(;;){
    change = false;
    for(i=0;i<count_min_label;i++){
      if(LUT[i] != LUT[LUT[i]])  LUT[i] = LUT[LUT[i]] ,change=true;
    }
    if(change == false) break;
  }

  //まわりで白かつ最も小さいラベルをrefに入れる
  if((ref>up && up > 0 || (ref== 0) &&  up >0 ))      ref=LUT[up] ,arround_white = true;
  if((ref>leftup && leftup > 0)  || (ref== 0 &&  leftup > 0))  ref=LUT[leftup],arround_white = true;
  if((ref>left && left > 0) || (ref== 0 &&  left > 0))   ref=LUT[left],arround_white = true;
      
  if(arround_white == false){//回りに白がなかったら
       labelnum[input.cols*y + x] = count_min_label;
       count_min_label++;
  }
      
  if(arround_white == true){//回りに白があったら
      labelnum[input.cols*y + x] = ref;//最も小さい値を入れる
      //隣接するラベルの番号が違う場合LUTに保存
      if(up >0 && up != ref) LUT[up] = ref;
      if(leftup >0 && leftup != ref) LUT[leftup] = ref;
      if(left >0 && left != ref) LUT[left] = ref;
  }

}

void label_nomal(Mat_<Vec3b> input, int *labelnum,int *LUT,int &count_min_label, int x,int y){
  
int ref,rightup,leftup,up,left;
bool arround_white = false;
    int i;  
  //ラベルの番号をレジスタに保存
  ref = labelnum[input.cols*y + x];
  rightup=labelnum[input.cols*(y-1) + x+1];
  up=labelnum[input.cols*(y-1) + x];
  leftup=labelnum[input.cols*(y-1) + x-1];
  left=labelnum[input.cols*y + x-1];

	bool change;

  //update LUT
	for(;;){
	  change = false;
	  for(i=0;i<count_min_label;i++){
	    if(LUT[i] != LUT[LUT[i]])  LUT[i] = LUT[LUT[i]] ,change=true;
	  }
	  if(change == false) break;
	}

  //まわりで白かつ最も小さいラベルをrefに入れる
  if((ref>rightup && rightup > 0)  || (ref== 0 &&  rightup > 0)) ref= LUT[rightup],arround_white = true;
  if((ref>up && up > 0 || (ref== 0) &&  up >0 ))      ref=LUT[up] ,arround_white = true;
  if((ref>leftup && leftup > 0)  || (ref== 0 &&  leftup > 0))  ref=LUT[leftup],arround_white = true;
  if((ref>left && left > 0) || (ref== 0 &&  left > 0))   ref=LUT[left],arround_white = true;
      
  if(arround_white == false){//回りに白がなかったら
  	   labelnum[input.cols*y + x] = count_min_label;
	    count_min_label++;
	    // printf("count:%d",count_min_label);
  }
      
  if(arround_white == true){//回りに白があったら
	    labelnum[input.cols*y + x] = ref;//最も小さい値を入れる
	    //隣接するラベルの番号が違う場合LUTに保存
      if(rightup >0 && rightup != ref) LUT[rightup] = ref;
      if(up >0 && up != ref) LUT[up] = ref;
      if(leftup >0 && leftup != ref) LUT[leftup] = ref;
      if(left >0 && left != ref) LUT[left] = ref;
  }

}



void first_path(Mat_<Vec3b> input, int *labelnum,int *LUT){

  int count_min_label=1;
  int x,y;
  int i;

  for(y=0; y<input.rows; y++){
    for(x=0; x< input.cols; x++){
      if(input(y,x)[0] > 100){//参照マスが白なら 値は適当
        
        //左端の時
        if(x==0){
          label_left(input, labelnum, LUT, count_min_label,x,y);
          continue;
        }
        
        //右端の時
        if(x==(input.cols-1)){
          label_right(input, labelnum, LUT, count_min_label,x,y);
          continue;
        }
        
        //上端の時
        if(y==0){
          label_up(input, labelnum, LUT, count_min_label,x,y);
          continue;
        }

        else
          label_nomal(input, labelnum,LUT, count_min_label,x,y);
          continue;    
      }
    }
  }

}




void labelling(Mat_<Vec3b> input, int *labelnum){


  int LUT[LUT_NUM];
  int i,j,x,y;

  //init LUT
  for(i=0;i<LUT_NUM;i++){
    LUT[i]=i;
  }



  first_path(input, labelnum,LUT);


  //2nd path 2ndpathはいらない?
  
  for(y=0; y<input.rows; y++){
    for(x=0; x< input.cols; x++){
      if(input(y,x)[0] > 100){//参照マスが白なら 値は適当
	       if(LUT[labelnum[input.cols*y  + x]]  !=  labelnum[input.cols*y  + x]) labelnum[input.cols*y  + x] = LUT[labelnum[input.cols*y  + x]];
      }
    }
  }
  
}


void labeling_main(Mat_<Vec3b> input ,char *filename){
  int *labelnum;
  int x,y;
  int i;  
  char name[100];

  //input=imread(argv[1],1);//color from file

  labelnum=(int *)calloc(input.rows * input.cols,sizeof(int));

  //printf("calloc ok\n");

  labelling(input,labelnum);

  //printf("labeling ok\n");
  
  paint(input,labelnum);

  for(i=0;i<100;i++){
    if(filename[i] == '.') break;
    name[i]=filename[i];
  }

  //imwrite(strcat(name,"_result.png"),input);//output file for file
  
}
