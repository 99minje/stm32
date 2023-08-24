#include "main.h"
#include "putchar.h"
#include <stdio.h>
#include <string.h>
#include "image01.h"
#include "image02.h"
#include "image03.h"
extern UART_HandleTypeDef huart2;
int gx, gy;
int _write(int file, char* p, int len)
{	int i,k;
	if (pr_dir)	{HAL_UART_Transmit(&huart2,(void *) p, len, 100);}
	else
	{	switch(font_size)
		{	case 12 : for (i=0;i<len;i++) putchar_e12(*p++); break;
			case 15 : for (i=0;i<len;i++) putchar_eng8x16(*p++); break;
			case 16	: for (i=0;i<len;i++) putchar_e16(*p++); break;
			case 20	: for (i=0;i<len;i++) putchar_e20(*p++); break;
			case 24 : for (i=0;i<len;i++) putchar_e24(*p++); break;
			default : k=f_color; f_color=RED; for (i=0;i<len;i++) putchar_e24(*p++);f_color=k;break;
		}
	}
	if (cur_y > MAX_Y - 24) cur_y = 0;
	return len;
}
#define MAX_BUFF  1000  // 25x40, 31x31, 320x3
uint8_t buff[2*MAX_BUFF];
void box(int x1,int y1,int x2,int y2,int color)
{	int w,h,i,j,k,m,dy,c1=color>>8;
	w = (x2-x1+1);	h=(y2-y1+1);  m=w*h;
	j = m % MAX_BUFF;
	dy = MAX_BUFF / w;
	k = h / dy;
	if (k) for(i=0;i<MAX_BUFF;i++) { buff[2*i+1]=c1;  buff[2*i]=color; }
	else   for(i=0;i<j;i++) { buff[2*i+1]=c1;  buff[2*i]=color; }
	while(y1<=y2-dy)
	{	BSP_LCD_FillRGBRect(0,0,buff,x1,y1,w,dy);
		y1 += dy;
	}
	dy=1+y2-y1;
	BSP_LCD_FillRGBRect(0,0,buff,x1,y1,w,dy);
}
void put_pixel(int x, int y, int color)
{	buff[1]=color>>8;  buff[0]=color;
	BSP_LCD_FillRGBRect(0,0,buff,x,y,1,1);
}
void LCD_Clear(unsigned int color) {box(0,0,MAX_X,MAX_Y,color); }
void X_line(unsigned int x, unsigned int y, unsigned int x1, unsigned int color)
{	int k,i,c1=color>>8;
	uint32_t Width, Height;
	if (x>x1) { k=x; x=x1; x1=k; }
	Width=x1-x+1; Height=1;
	for (i=0;i<Width;i++) { buff[2*i+1]=c1;  buff[2*i]=color; }
	BSP_LCD_FillRGBRect(0,0,buff,x,y,Width,Height);
}
void Y_line(unsigned int x, unsigned int y, unsigned int y1, unsigned int color)
{	int k,i,c1=color>>8;;
	uint32_t Width, Height;
	if (y>y1) { k=y; y=y1; y1=k; }
	Width= 1; Height=y1-y+1;
	for (i=0;i<Height;i++) { buff[2*i+1]=c1;  buff[2*i]=color; }
	BSP_LCD_FillRGBRect(0,0,buff,x,y,Width,Height);
}
int abs(int x) { if (x>=0) return (x); else  return(-x); }
void line(int x1,int y1,int x2,int y2,int color)
{	int dx,dy,i,j,k;
	dx = x1-x2;	dy = y1-y2;
	if (abs(dx)>abs(dy))
	{	if (x1<x2)  { j = x1; k = x2; }	else  { j = x2; k = x1 ; }
		for ( i=j; i<=k ; i++ ) put_pixel(i,((i-x1)*dy)/dx + y1,color);
	} else
	{	if (y1<y2) { j = y1; k = y2; }	else  { j = y2; k = y1; }
		for ( i=j; i<=k ; i++ ) put_pixel(((i-y1)*dx)/dy + x1,i,color);
	}
}
void disp_Rect(int sX,int sY,int width,int height)
{	BSP_LCD_FillRGBRect(0,0,buff,sX,sY,width,height);
}

int colorpalette(int position, const char bmp[])
{

//  position = position*3+54;

  union palette
  {
    unsigned int i;
    char c[3];
  };

  union palette color;

  color.c[2] =bmp[position++];
  color.c[1] =bmp[position++];
  color.c[0] =bmp[position++];

  short rgbBlue = ((color.i & 0xff0000) >> 16);
  short rgbGreen = ((color.i & 0x00ff00) >> 8);
  short rgbRed = ((color.i & 0x0000ff) >> 0);

  rgbRed = rgbRed >> 3;
  rgbGreen = rgbGreen >> 2;
  rgbBlue = rgbBlue >> 3;

 int colord = ((rgbRed<<11)|(rgbGreen<<5)|(rgbBlue));

  return colord;
}

int bf4(const char bmp[], int pos)
{
  union bm
  {
    unsigned int i;  // 4 byte
    char c[4];
  };

  union bm info;
  int k;

  for(k=0; k<4; k++) info.c[k]=bmp[pos++];

  return info.i;
}
int biWidth ,biHeight, bmx=0, bmy=0;

void ITBMP_draw(const char bmp[], int ix, int iy)
{
  int i;
  int bfOffBits = bf4(bmp,10);//鍮꾪듃留??곗씠?곗쓽 ?쒖옉 ?꾩튂
  biWidth = bf4(bmp,18);    //480-1
  biHeight = bf4(bmp,22);   //272-1

  i = bfOffBits;

  while(i < 1200){

	  if(bmx>biWidth-1){bmx=0;bmy++;}
	  put_pixel(bmx+ix, biHeight-1-bmy+iy, colorpalette(i, bmp));
	  bmx++;
      i+=3;

  }
}

void BMP_draw(const char bmp[], int ix, int iy)
{
  int i=0, j=0, s=0;

  while(i < 1200){
	  	  if(bmx==biWidth && bmy==biHeight) break;
	      if(bmx>biWidth-1){bmx=0;bmy++;}
	      put_pixel(bmx+ix, biHeight-1-bmy+iy, colorpalette(i, bmp));
	      bmx++;
      i+=3;
  }
}

void ITimg1_draw(const char bmp[], int ix, int iy)
{
  int i;
  int bfOffBits = bf4(bmp,10);//鍮꾪듃留??곗씠?곗쓽 ?쒖옉 ?꾩튂
  biWidth = bf4(bmp,18);    //480-1
  biHeight = bf4(bmp,22);   //272-1

  i = bfOffBits;

  while(i < 1200){

	  if(bmx>biWidth-1){bmx=0;bmy++;}
	  put_pixel(bmx+ix, biHeight-1-bmy+iy, bmp[i+1]<<8 | bmp[i]);
	  bmx++;
      i+=2;

  }
}

void img1_draw(const char bmp[], int ix, int iy)
{
  int i=0, j=0, s=0;

  while(i < 1200){
	  	  if(bmx==biWidth && bmy==biHeight) break;
	      if(bmx>biWidth-1){bmx=0;bmy++;}
	      put_pixel(bmx+ix, biHeight-1-bmy+iy, bmp[i+1]<<8 | bmp[i]);
	      bmx++;
      i+=2;
  }
}

void BMP_draw1(const char bmp[], int ix, int iy)
{
  int i=0, j=0, s=0, bmx=0, bmy=0, number, number2;
  int bfOffBits = bf4(bmp,10);//鍮꾪듃留??곗씠?곗쓽 ?쒖옉 ?꾩튂
  int biWidth = MAX_X; //bf4(bmp,18);    //480-1
  int biHeight = MAX_Y; //bf4(bmp,22);   //272-1

  while(1){
    number = bmp[bfOffBits+i];
    if(number == 0x00)
    {
      number2 = bmp[bfOffBits+i+1];
      if(number2 == 0x00){i+=2;bmx=0;bmy++;}
      if(number2 == 0x01){i+=2;break;}
      if(number2 == 0x02){
    	  bmx+=bmp[bfOffBits+i+2]; bmy += bmp[bfOffBits+i+3];
    	  if(bmx>biWidth-1){bmx=0;bmy++;}
    	  i+=4;
      }
      if(number2>=3 && number2<=255)
      {

        for(j=0; j<number2; j++){
          if(bmx>biWidth-1){bmx=0;bmy++;}
          put_pixel(bmx+ix, biHeight-1-bmy+iy, colorpalette( bmp[bfOffBits+i+2+j], bmp)  );
          bmx++;
        }
        if(number2%2==0) i=i+number2+2; else i=i+number2+3;
      }
    }
    else
    {
      number2 = bmp[bfOffBits+i+1];
      for(j=0; j<number; j++){
        if(bmx>biWidth-1){bmx=0;bmy++;}
        put_pixel(bmx+ix, biHeight-1-bmy+iy, colorpalette(bmp[bfOffBits+i+1], bmp));
        bmx++;
      }
      i+=2;
    }
  }
  gx = bmx; gy = bmy;
}

void g071bsp(void)
{	int i,len;
	char str[20];

	gx = 0; gy = 0;
	pr_dir=1;  // Output ==> Serial
	printf("\n\r\nProgram Start!!!\n\r");
	i=BSP_LCD_Init(0,3);  printf("BSP_LCD_Init=%d\n\r",i);
	i=BSP_LCD_DisplayOn(0); printf("BSP_LCD_DisplayOn=%d\n\r",i);
	LCD_Clear(BLUE);
	/*for (i=0;i<32;i++)   box(i*8,160,(1+i)*8,179,i);
	for (i=0;i<64;i++)   box(i*4,180,(i+1)*4,199,i<<5);
	for (i=0;i<32;i++)   box(i*8,200,(i+1)*8,219,i<<11);
	for (i=0;i<8;i++)    box(40*i,220,(i+1)*40,239,COLOR_TABLE[i]);
	printf("LCD printf Test !!\n\r");
	pr_dir=0;  f_color=YELLOW;  b_color=BLUE; cur_x=20; cur_y=20;
	sprintf(str,"putchar_ test!!\n"); len=strlen(str);
	for (i=0;i<len;i++)  putchar_e16(str[i]);
	font_size = 12;  printf("Font 12y\n\r");
	font_size = 15;  printf("Font 8x16 Wy\n\r");
	font_size = 16;  printf("Font 16 Wy\n\r");
	font_size = 20;  printf("Font 20 Wy\n\r");
	font_size = 24;  printf("Font 24 Wy\n\r");
	font_size = 15;*/
	pr_dir=1;
	i=BSP_MEM_Init(0); printf("BSP_MEM_Init=%d\n\r",i);
//	i=(int) image02;  printf("image 2 Address = %X\n\r",i);
//	i=BSP_MEM_ReadData(0,buff,i,20);printf("BSP_MEM_ReadData = %d\n\r",i);
//	for (i=0;i<20;i++) printf("%02X ",buff[i]);
//	printf("\n\r");
	i=(int) image01;  printf("image 2 Address = %X\n\r",i);

	BSP_MEM_ReadData(0,buff,i,1200);
	ITimg1_draw(buff,0,0);

	for(int j = 1; j <200; j++){
		BSP_MEM_ReadData(0,buff,i+1200*j,1200); //printf("BSP_MEM_ReadData = %d\n\r",i);
		img1_draw(buff,0, 0);
	/*for (i=0;i<2;i++) printf("%02X ",buff[i]);
	printf("\n\r");*/
	}

	i=BSP_MEM_EnableMemoryMappedMode(0);printf("BSP_MEM_EnableMemoryMappedMode = %d\n\r",i);

//	while(1)
//	{
//	}
}
