#include <stdio.h>
#include <graphics.h>
#include <math.h>

#define  PI180   3.1415926/180
#define  SCALE   250

int main (void)
{
	const int  MAX_X=220, MAX_Y=176;
	const int pointx[6]={-7,-10,-7,90,95,90};
	const int pointy[6]={1,0,-1,-3,0,3};
	int sint[200],cost[200];
	int CX=110, CY=150, R=100;
	int i,j,k,x,y,x1,y1;
	char str[50];
	
	for (i=0;i<200;i++) { sint[i] = SCALE * sin(i*PI180); cost[i] = SCALE * cos(i*PI180); }
	initwindow(MAX_X+20,MAX_Y+20);
	setbkcolor(BLUE);
	cleardevice(); 
	setcolor(YELLOW);
	x=y=x1=0;  
	y1=MAX_Y;	line(x,y,x1,y1);
	x=MAX_X; y=MAX_Y;	line(x,y,x1,y1);
	y1=0;x1=MAX_X;		line(x,y,x1,y1);
	x=0;	y=0;	line(x,y,x1,y1);
	settextstyle(8,0,2);
	sprintf(str,"SPEED Meter");
	outtextxy(10,10,str);
	x=CX-R; y=CY; x1=CX+R; y1=CY;
	line(x,y,x1,y1);
	for (i=0;i<180;i++) 
	{	x=x1; y=y1; x1=CX+(R*cost[i])/SCALE; y1=CY-(R*sint[i])/SCALE;
		line(x,y,x1,y1);
	}
	i=10; k=1;
	while (!kbhit())
	{	setcolor(BLUE);
		for (j=0;j<6;j++)
		{	x=(pointx[j]*cost[i]-pointy[j]*sint[i])/SCALE+CX;        y= CY-(pointx[j]*sint[i]+pointy[j]*cost[i])/SCALE;
			if(j<5) {x1=(pointx[j+1]*cost[i]-pointy[j+1]*sint[i])/SCALE+CX; y1= CY-(pointx[j+1]*sint[i]+pointy[j+1]*cost[i])/SCALE; }
			else {x1=(pointx[0]*cost[i]-pointy[0]*sint[i])/SCALE+CX; y1= CY-(pointx[0]*sint[i]+pointy[0]*cost[i])/SCALE;}
			line(x,y,x1,y1);
		}
		if (k) {	if (i<180) i++;	else k = 0;  }
		  else { 	if (i>1)  i--; else k = 1;  }
		setcolor(YELLOW);
		for (j=0;j<6;j++)
		{	x=(pointx[j]*cost[i]-pointy[j]*sint[i])/SCALE+CX;        y= CY-(pointx[j]*sint[i]+pointy[j]*cost[i])/SCALE;
			if(j<5) {x1=(pointx[j+1]*cost[i]-pointy[j+1]*sint[i])/SCALE+CX; y1= CY-(pointx[j+1]*sint[i]+pointy[j+1]*cost[i])/SCALE; }
			else {x1=(pointx[0]*cost[i]-pointy[0]*sint[i])/SCALE+CX; y1= CY-(pointx[0]*sint[i]+pointy[0]*cost[i])/SCALE;}
			line(x,y,x1,y1);
		}
		delay(10);
	}
	getch();
	closegraph();
}


