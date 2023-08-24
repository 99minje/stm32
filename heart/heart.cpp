#include <stdio.h>
#include <math.h>
#include <graphics.h>
int adv;
int buf[17] = {0,};
int gx = 11, gy = 160;
#define MAX_X  480 
#define MAX_Y  320
#define HR_MAX 200
#define HR_MIN 60
#define T_MAX 60000/HR_MIN
#define T_MIN 60000/HR_MAX

static int heart_buf[17] ={0,};
static int count; static int flag, sum;
int maxx, minn, s_max=0, s_min;
int first = 1;
int ADC_Val;
int ready=0;
int nort[20];
int tm, diss; int sel = 0;
int peak_val;
int diff;
int s_flag = 1;
int ch, dis_flag ,distance;

void peak(void){
	
	sum = sum - heart_buf[flag];
	heart_buf[flag] = adv;
	sum = sum + heart_buf[flag];

	flag = (flag < 16) ? flag + 1 : 0;

	diff = abs(nort[ready] - sum);
	nort[ready] = sum;
	ready = (ready > 18) ? 0 : ready + 1;

	maxx = (sum > maxx) ? diff : maxx;
	minn = (sum < minn) ? diff : minn;

	if(count > 10){
			s_max = maxx;
			s_min = minn;

			maxx = sum;
			minn = sum;
			count = 0;
		}

	count++; tm++; sel++; diss++;
}

int cutt; int fst_check = 1;


void heart_main(void){
	
		if(s_max){
		if(gx+1 > MAX_X) {gx = 11; setcolor(BLUE); line(gx,30,gx,320);}

		else {
		setcolor(BLUE);
		line(gx,30,gx,320);
		//setcolor(YELLOW);
		//line(gx,320-(s_min/150), gx,320- (s_max/150));
		putpixel(gx,320-diff/100,YELLOW);
	}
		gx = gx + 1;

		s_max = 0;

	}
}

void init_peak(void){
	cutt = 320; fst_check = 1; sel = 0; tm = 0; diss = 0; s_flag = 1; dis_flag = 0; distance = 0;
}

void cal_peak(void){
	int temp;
	int heart_val = 320- (diff/100);
	

	if(tm > T_MAX) { init_peak(); } //noise

	else{
		if(heart_val < cutt) { 
			cutt = heart_val; sel = 0; tm = 0;
			
		}
		
		if((sel > T_MIN) && (fst_check == 1)) { temp = cutt; diss = 0; cutt = cutt / 2; fst_check = 0; sel = 0;}
		if((sel > T_MIN) && (fst_check == 0)) { init_peak(); peak_val = temp; }
	}
}

void draw_peak(void){
	
	int heart_val = 320- (diff/100);

	if(tm > T_MAX) { init_peak(); } //noise
	
	else{
		if(heart_val < cutt) { 
			cutt = heart_val; tm = 0; if(s_flag == 1) sel = 0;
			if(dis_flag == 0) distance = 0;
		}
		
		if((sel > T_MIN) && (fst_check == 1)) { ch = cutt; diss = 0; cutt = 250; fst_check = 0; sel = 0; s_flag = 0;  dis_flag = 1; }
		if((sel > T_MIN) && (fst_check == 0)) { peak_val = ch; ch = 0; printf("%d\n\r",distance); init_peak();}
	}
	
	distance++;
	
	//printf("%d\n\r",ch);
	
	if(s_max){
	if(gx+1 > MAX_X) {gx = 11; setcolor(BLUE); line(gx,30,gx,320);}

	else {
		setcolor(BLUE);
		line(gx,30,gx,320);
		putpixel(gx,ch, YELLOW);
	
	}
	gx = gx + 1;
	s_max = 0;

	}
}

int main (void)
{
	init_peak();
	gx = 11; gy = 160;
	
	char str[50];
   FILE* fs;
    fs = fopen("hel.txt", "r");
	int x,y,x1,y1;
	initwindow(MAX_X+15,MAX_Y+15);
	setbkcolor(BLUE);
	cleardevice(); 
	setcolor(YELLOW);
	x=y=x1=10;  
	y1=MAX_Y;	line(x,y,x1,y1);
	x=MAX_X; y=MAX_Y;	line(x,y,x1,y1);
	y1=10;x1=MAX_X;		line(x,y,x1,y1);
	x=10;	y=10;	line(x,y,x1,y1);
	settextstyle(8,0,2);
	sprintf(str,"Heartrate");
	outtextxy(10,10,str);
	
	x = 11; y = 160;
	int st = 0;
	
	while (!feof(fs))
	{	fscanf(fs,"%d",&adv);
		//printf("%d\n\r",320- (diff/100));
		peak();
		//heart_main();
		draw_peak();
	}
	getch();
	closegraph();
}


