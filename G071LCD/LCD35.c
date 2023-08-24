// 3.5 In LCD with ILI9481
#include <stdio.h>
#include "main.h"
#include "app_fatfs.h"
#include "e8x12.h"
#include "eng16x24.h"
#include "stdlib.h"
#include "math.h"
#include "HAN16.h"
#include "kssm.h"
#include "string.h"
#include "E8X16.h"
#include "E12X24.h"
#include "E16X24.h"
#include "E24X40.h"
#include "song.h"
#include "straw.h"
#include "3.h"

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim1;

#define sclk_high() SD_SCK_GPIO_Port->BSRR=SD_SCK_Pin
#define sclk_low() SD_SCK_GPIO_Port->BRR=SD_SCK_Pin
#define do_high() SD_DO_GPIO_Port->BSRR=SD_DO_Pin
#define do_low() SD_DO_GPIO_Port->BRR=SD_DO_Pin
#define read_sd() SD_DI_GPIO_Port->IDR & SD_DI_Pin
#define ss_high() SD_SS_GPIO_Port->BSRR=SD_SS_Pin
#define ss_low() SD_SS_GPIO_Port->BRR=SD_SS_Pin
#define Pin_High(Port,bit)  Port->BSRR = bit
#define Pin_Low(Port,bit)  Port->BSRR = (bit << 16)
#define MAX_X 479
#define MAX_Y 319
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define HR_MAX 180
#define HR_MIN 80
#define T_MAX 60000/HR_MIN
#define T_MIN 60000/HR_MAX

const int COLOR_TABLE[8] = {BLACK,BLUE,RED ,GREEN ,CYAN, MAGENTA,YELLOW ,WHITE };
int gx = 1; int gy =1; int fsize = 2;
void Lcd_Write_Byte(int d)
{	Pin_Low(LCD_WR_GPIO_Port,LCD_WR_Pin);
	if (d & 0x01) Pin_High(LCD_D0_GPIO_Port,LCD_D0_Pin); else  Pin_Low(LCD_D0_GPIO_Port,LCD_D0_Pin);
	if (d & 0x02) Pin_High(LCD_D1_GPIO_Port,LCD_D1_Pin); else  Pin_Low(LCD_D1_GPIO_Port,LCD_D1_Pin);
	if (d & 0x04) Pin_High(LCD_D2_GPIO_Port,LCD_D2_Pin); else  Pin_Low(LCD_D2_GPIO_Port,LCD_D2_Pin);
	if (d & 0x08) Pin_High(LCD_D3_GPIO_Port,LCD_D3_Pin); else  Pin_Low(LCD_D3_GPIO_Port,LCD_D3_Pin);
	if (d & 0x10) Pin_High(LCD_D4_GPIO_Port,LCD_D4_Pin); else  Pin_Low(LCD_D4_GPIO_Port,LCD_D4_Pin);
	if (d & 0x20) Pin_High(LCD_D5_GPIO_Port,LCD_D5_Pin); else  Pin_Low(LCD_D5_GPIO_Port,LCD_D5_Pin);
	if (d & 0x40) Pin_High(LCD_D6_GPIO_Port,LCD_D6_Pin); else  Pin_Low(LCD_D6_GPIO_Port,LCD_D6_Pin);
	if (d & 0x80) Pin_High(LCD_D7_GPIO_Port,LCD_D7_Pin); else  Pin_Low(LCD_D7_GPIO_Port,LCD_D7_Pin);
	Pin_High(LCD_WR_GPIO_Port, LCD_WR_Pin);
}
void Lcd_Write_Com(int comm)  
{   Pin_Low(LCD_RS_GPIO_Port,LCD_RS_Pin);
	Lcd_Write_Byte(comm);
	Pin_High(LCD_RS_GPIO_Port,LCD_RS_Pin);
}
void window_set(int x1,int y1,int x2,int y2)
{   Lcd_Write_Com(0x2a); 	Lcd_Write_Byte(x1>>8);	Lcd_Write_Byte(x1);	Lcd_Write_Byte(x2>>8);	Lcd_Write_Byte(x2);
    Lcd_Write_Com(0x2b);	Lcd_Write_Byte(y1>>8);	Lcd_Write_Byte(y1);	Lcd_Write_Byte(y2>>8);	Lcd_Write_Byte(y2);
	Lcd_Write_Com(0x2c); 							 
}

void put_color(int color) { Lcd_Write_Byte(color>>8); Lcd_Write_Byte(color); }
void put_pixel(int x, int y, int color)
{	unsigned char x1=x>>8,y1=y>>8,x2=x&0xff,y2=y&0xff;
	Lcd_Write_Com(0x2a); 	Lcd_Write_Byte(x1);	Lcd_Write_Byte(x2);	Lcd_Write_Byte(x1);	Lcd_Write_Byte(x2);
	Lcd_Write_Com(0x2b);	Lcd_Write_Byte(y1);	Lcd_Write_Byte(y2);	Lcd_Write_Byte(y1);	Lcd_Write_Byte(y2);
	Lcd_Write_Com(0x2c);	Lcd_Write_Byte(color>>8);	Lcd_Write_Byte(color);
}

void put_8x12font(int x, int y, int num, int color){
	int n = (num) * 12;
	for(int j = 0; j <12; j++){
	for(int i = 0; i < 8; i++){
		if(e8x12[n+j] & (0x80 >> i)) put_pixel(x+i, y+j, color);
	}}
}

void put_16x24font(int x, int y, int num, int color){
	int n = (num) * 24;
	for(int j = 0; j <24; j++){
	for(int i = 0; i < 16; i++){
		if(eng16x24[n+j] & (0x8000 >> i)) put_pixel(x+i, y+j, color);
	}}
}

void LCD_Clear(unsigned int color)
{	int ci,cj;
	int m;
	window_set(0,0,MAX_X,MAX_Y);
	ci = color >> 8; cj = color & 0xff;
	m = (MAX_X+1) * (MAX_Y+1);
	for(;m>0;m--)  { Lcd_Write_Byte(ci); Lcd_Write_Byte(cj);   }
}
void Lcd_Init(void)
{	Pin_High(LCD_RD_GPIO_Port,LCD_RD_Pin);
	Pin_High(LCD_RST_GPIO_Port,LCD_RST_Pin);	HAL_Delay(5);
	Pin_Low(LCD_RST_GPIO_Port,LCD_RST_Pin);		HAL_Delay(15);
	Pin_High(LCD_RST_GPIO_Port,LCD_RST_Pin);	HAL_Delay(50);
	Pin_High(LCD_CS_GPIO_Port,LCD_CS_Pin);
	Pin_High(LCD_WR_GPIO_Port,LCD_WR_Pin);
	Pin_Low(LCD_CS_GPIO_Port,LCD_CS_Pin);
	Lcd_Write_Com(0x11);						HAL_Delay(120);	// Exit Sleep Mode
	Lcd_Write_Com(0x3A);	Lcd_Write_Byte(0x55);		// Set Pixel Format 16bit
	Lcd_Write_Com(0x36);	Lcd_Write_Byte(0x28);		// Set Address Mode  BGR Order
	Lcd_Write_Com(0x29);		// Set Display On
	LCD_Clear(WHITE);

		// Positive Gamma Control
	Lcd_Write_Com(0xE0);
	Lcd_Write_Byte(0x0F);
	Lcd_Write_Byte(0x1B);
	Lcd_Write_Byte(0x18);
	Lcd_Write_Byte(0x0B);
	Lcd_Write_Byte(0x0E);
	Lcd_Write_Byte(0x09);
	Lcd_Write_Byte(0x47);
	Lcd_Write_Byte(0x94);
	Lcd_Write_Byte(0x35);
	Lcd_Write_Byte(0x0A);
	Lcd_Write_Byte(0x13);
	Lcd_Write_Byte(0x05);
	Lcd_Write_Byte(0x08);
	Lcd_Write_Byte(0x03);
	Lcd_Write_Byte(0x00);

	// Negative Gamma Correction
	Lcd_Write_Com(0XE1);
	Lcd_Write_Byte(0x0F);
	Lcd_Write_Byte(0x3A);
	Lcd_Write_Byte(0x37);
	Lcd_Write_Byte(0x0B);
	Lcd_Write_Byte(0x0C);
	Lcd_Write_Byte(0x05);
	Lcd_Write_Byte(0x4A);
	Lcd_Write_Byte(0x24);
	Lcd_Write_Byte(0x39);
	Lcd_Write_Byte(0x07);
	Lcd_Write_Byte(0x10);
	Lcd_Write_Byte(0x04);
	Lcd_Write_Byte(0x27);
	Lcd_Write_Byte(0x25);
	Lcd_Write_Byte(0x00);
}

void line(int x1, int y1, int x2, int y2, int color){
	double lenx = x2 - x1;
	double leny = y2 - y1;
	double ratio, x_ver, y_ver, ovf = 0;
	if(lenx == 0) ovf = 1;
	else ratio = leny / lenx;
	double cont = y2 - x2 * ratio;
	double len, sx, sy, tx1,ty1 , tx2,ty2 = 0;

	if(abs(lenx) >= abs(leny)) {len = abs(lenx); sx = 1;}
	else {len = abs(leny); sy = 1;}

	if(x1 > x2) x_ver = 1;
	if(y1 > y2) y_ver = 1;

	if(sx == 1){
		for(int i = 0; i < len; i++){
			if(x_ver == 1)tx1 = x1 - i;
			else tx1 = x1 + i;
			ty1 = tx1 * ratio + cont;
			put_pixel(tx1, ty1, color);
	}}

	if(sy == 1){
		for(int i = 0; i< len; i++){
			if(y_ver == 1) ty2 = y1 - i;
			else ty2 = y1 + i;

			if(ratio == 0) tx2 = x1;
			else {tx2 = (ty2 - cont) / ratio;}
			put_pixel(tx2, ty2, color);
		}
	}
	put_pixel(x2,y2,color);
}

double pi = 3.14;

void circle(int x1, int y1, int r, int color){
	int x2, y2;

	for(int i = 0; i < 36; i++){
		x2 = x1 + r*cos(pi/18 * i);
		y2 = y1 + r*sin(pi/18 * i);
		line(x1,y1,x2,y2,color);
	}
}

void lcd_main(void)
{
	Lcd_Init();
	int x8=0; int x16=0;
	int y8 = 2; int y16 = 30;
	
	/*for(int num8 = 0; num8 < 96; num8++){
		put_8x12font(x8, y8, num8, BLACK);
		x8 = x8 + 9;
		if(num8 >95) {num8 = 0; x8 =0; y8 = 0;}
		if(x8 > 471) {x8 = 0; y8 = y8 + 13;}
	}

	for(int num16 = 0; num16 < 96; num16++){
		put_16x24font(x16, y16, num16, BLACK);
		x16 = x16 + 17;
			if(num16 >95) {num16 = 0; x16 =0; y16 = 0;}
			if(x16 > 471) {x16 = 0; y16 = y16 + 25;}
		}*/


	//line(0,0,10,205,BLACK);
	//line(0,0,150,15,BLACK);
	//line(100,200,10,20,BLACK);
	//line(100,20,15,205,BLACK);
	//line(10,200,150,20,BLACK);

	circle(100,200, 20,BLACK);

	while(1);

	//circle(300, 200, 30, BLACK);

}

const unsigned char cho[] ={0,0,0,0,0,0,0,0,1,3,3,3,1,2,4,4,4,2,1,3,0};        //받침 없는 초성 벌 수
const unsigned char cho2[]={5,5,5,5,5,5,5,5,6,7,7,7,6,6,7,7,7,6,6,7,5};      //받침 있는 초성 벌 수
//const unsigned char jong[]={0,1,0,1,0,1,0,1,1,0,1,0,1,1,0,1,0,1,1,0,0};
const unsigned char jong[]={0,2,0,2,1,2,1,2,3,0,2,1,3,3,1,2,1,3,3,1,1};
int forcolor=BLACK, bakcolor=WHITE;
int pr_size=0,dispcolor;

//============================8X12 FONT=========================================
int pute8x12(int ch)
{
#define bitx  8
#define bity  12
#define maxfont  96

  int i,k,comp,gc;

  if (ch=='\n') { gx=0; gy += bity; }
  if ( (ch<32) && (ch>127) ) return(1);

  ch = bity * (ch-32);
  for ( k=0;k<bity ;k++ )
  {   gc = e8x12[ch++];

  comp = 0x80;
  for (i=0;i<bitx ;i++ )
  {    dispcolor = (gc & comp) ? forcolor : bakcolor;
  put_pixel(gx+i,gy+k,dispcolor);
  comp = comp >> 1;
  }
  }
  if (gx+bitx+bitx<MAX_X) { gx = gx + bitx;  }  else { gx=0; gy = gy + bity; }

  return(ch);
}

//============================8X16 FONT=========================================
int pute8x16(int ch)
{
#define bitx  8    // 가로 비트
#define bity  16   // 세로 비트
#define maxfont  96 // ASCII CODE 출력 가능한 갯수(' ' ~ '~')

  int i,k,comp,gc;

  if (ch=='\n') { gx=0; gy += bity; }

  if ( (ch<32) && (ch>127) ) return(1);  // ASCII CODE 검사
  ch = bity * (ch-32);                   // 해당 글자 바이트 위치로 이동
  for ( k=0;k<bity ;k++ ){
    gc = e8x16[ch++];
    comp = 0x80;
    for (i=0;i<bitx ;i++ ){
      dispcolor = (gc & comp) ? forcolor : bakcolor;
      put_pixel(gx+i,gy+k,dispcolor);
      comp = comp >> 1;
    }
  }

  if (gx+bitx+bitx<MAX_X) { gx = gx + bitx;  }  else { gx=0; gy = gy + bity; }

  return(ch);
}

//============================12X24 FONT========================================
int pute12x24(int ch)
{
#define bitx  12
#define bity  24
#define maxfont  96

  unsigned int i,k;
  unsigned int comp;

  union inch
  {    short unsigned int i;
  char c[2];
  };

  union inch gc;

  if (ch=='\n') { gx=0; gy += bity; }
  if ( (ch<32) && (ch>127) ) return(0);
  ch = 2 * bity * (ch-32);

  for ( k=0;k<bity ;k++ )
  {
    gc.c[1] = e12x24[ch++];
    gc.c[0] = e12x24[ch++];
    comp = 0x8000;
    for (i=0;i<bitx ;i++ )
    {    dispcolor = (gc.i & comp) ? forcolor : bakcolor;
    put_pixel(gx+i,gy+k,dispcolor);
    comp = comp >> 1;
    }
  }
  if (gx+bitx+bitx<MAX_X) { gx = gx + bitx;  }  else { gx=0; gy = gy + bity; }
  return(1);
}


//============================e16X24 FONT=========================================
int pute16x24(int ch)
{
#define bitx  16
#define bity  24
#define maxfont  96

  unsigned int i,k;
  unsigned int comp;
  union inch
  {    short unsigned int i;
  char c[2];
  };
  union inch gc;

  if (ch=='\n') { gx=0; gy += bity; }
  if ( (ch<32) && (ch>127) ) return(0);
  ch = 2 * bity * (ch-32);
  for ( k=0;k<bity ;k++ )
  {
    gc.c[1] = e16x24[ch++];
    gc.c[0] = e16x24[ch++];
    comp = 0x8000;
    for (i=0;i<16 ;i++ )
    {    dispcolor = (gc.i & comp) ? forcolor : bakcolor;
    put_pixel(gx+i,gy+k,dispcolor);
    comp = comp >> 1;
    }
  }
  if (gx+bitx+bitx<MAX_X) { gx = gx + bitx;  }  else { gx=0; gy = gy + bity; }
  return(1);

}

//============================e24X40 FONT=========================================
int pute24x40(int ch)
{
#define bitx  24
#define bity  40
#define maxfont  96

  unsigned int i,k;
  long int comp;
  union inch
  {
    long int i;
    char c[3];
  };
  union inch gc;

  if (ch=='\n') { gx=0; gy += bity; }
  if ( (ch<32) && (ch>127) ) return(0);

  ch = 3 * bity * (ch-32);
  for ( k=0;k<bity ;k++ )
  {
    gc.c[2] = e24x40[ch++];
    gc.c[1] = e24x40[ch++];
    gc.c[0] = e24x40[ch++];
    comp = 0x800000;
    for (i=0;i<bitx ;i++ )
    {    dispcolor = (gc.i & comp) ? forcolor : bakcolor;
    put_pixel(gx+i,gy+k,dispcolor);
    comp = comp >> 1;
    }
  }
  if (gx+bitx+bitx<MAX_X) { gx = gx + bitx;  }  else { gx=0; gy = gy + bity; }
  return(1);

}

void puthan16(unsigned short int ch1, unsigned short int ch2, unsigned short int ch3)
{
#define hbit16x  16
#define hbit16y  16
#define ch32byte  32             // 한 글자당 바이트수 = 2 x 16

  unsigned int i,k;
  unsigned int comp,chout;
  union inch
  {
    short unsigned int i;
    char c[2];
  };

  union inch gch1, gch2, gch3;

  ch1 = ch32byte * ch1;
  ch2 = ch32byte * ch2;
  ch3 = ch32byte * ch3;

  for ( k=0;k<hbit16y ;k++ )
  {
    gch1.c[1] = han16[ch1++];
    gch1.c[0] = han16[ch1++];

    gch2.c[1] = han16[ch2++];
    gch2.c[0] = han16[ch2++];

    if ((ch3)){
      gch3.c[1] = han16[ch3++];
      gch3.c[0] = han16[ch3++];
      chout = gch1.i | gch2.i | gch3.i ;
    }
    else chout = gch1.i | gch2.i ;

    comp = 0x8000;
    for (i=0;i<hbit16x ;i++ )
    {
      dispcolor = (chout & comp) ? forcolor : bakcolor;
      put_pixel(gx+i,gy+k,dispcolor);
      comp = comp >> 1;
    }
  }
  if (gx+hbit16x+hbit16x<MAX_X) { gx = gx + hbit16x;  }  else { gx=0; gy = gy + hbit16y; }
}

int putchar(int ch)
{
  unsigned short int in_ch,ch_kssm,i,j,k,ch1,ch2,ch3;
  static unsigned short int ch_h=0;

  in_ch = 0x00ff & ch;

  if (in_ch < 0x80) //        ascii code 0~127               글자가 영어인 경우
  {
    //영문 폰트 사이즈
    switch (fsize)
    {
    case 1 : pute8x12(ch); break;
    case 2 : pute8x16(ch); break;
    case 3 : pute12x24(ch); break;
    case 4 : pute16x24(ch); break;
    default : pute24x40(ch); break;
    }
    ch_h = 0;
  }
  // 글자가 한글인 경우
  else
  {
    if (ch_h){
      if ( ( ch_h >= 0xB0) && (ch_h<=0xC8) ){	// 완성형 한글
        if (in_ch > 0xA0){

          i= 94*(ch_h-0x00B0)+in_ch-0xA1;
          ch_kssm = KSSM[i];				// 완성형 >> 조합형 한글

          ch1 = (ch_kssm >> 10 ) & 0x001F;		// 초성
          ch2 = (ch_kssm >>  5 ) & 0x001F;		// 중성
          ch3 = (ch_kssm & 0x001f);			// 종성

          if (ch1 > 1) ch1 = ch1 - 2;			// 미사용 영역 제거
          if (ch2 > 0x17) ch2 = ch2 - 9;		// 초성&중성은 0 시작.
          else if ( ch2 > 0x0f) ch2 = ch2 - 7;
          else if ( ch2 > 0x07) ch2 = ch2 - 5;
          else ch2 = ch2 - 3;
          if (ch3 > 0x12) ch3 = ch3 - 2;		// 종성은 1 시작 (없는경우 0)
          else if ( ch3 > 1) ch3 = ch3 - 1;
          else ch3 = 0;

          if (ch3 == 0){                                    //먼저 종성이 있는 지 없는 지를 조사
            i = cho[ch2];                                  //초성
            if ( ch1==0 || ch1==15 ) j=0;              //종성
            else j=1;
          }
          else
          {
            i=cho2[ch2];                                  //초성 */
            if (ch1==0 || ch1==15) j=2;                //종성
            else j=3;
            k=jong[ch2];
          }

          ch1 = i * 19 + ch1;
          ch2 = (8*19) + (j*21) + ch2;
          if (ch3) ch3 = (8*19) + (4*21) +  (k*27) +ch3 -1;

          puthan16(ch1,ch2,ch3);
        }
      }
      ch_h = 0;
    }
    else ch_h = in_ch;
  }
  return(ch);
}

void han_main(void){
	Lcd_Init();
	gx=100;gy=100;
	//putchar(0xb0);
	//putchar(0xa1);
	//int len = strlen(straw);
/*	for(int i = 0; i < 150; i++){
	putchar(forward[i]);
	}*/

	//for(int i = 0; i < len; i++){
	//putchar(straw[i]);
	//}

	while(1){

	}
}

int colorpalette(int position, const char bmp[])
{

  position = position*3+54;

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

 int colord = ((rgbRed<<7)|(rgbGreen<<3)|(rgbBlue));



  return colord;
}

unsigned long int bf4(const char bmp[], int pos)
{
  union bm
  {
    unsigned int i;  // 4 byte
    char c[3];
  };

  union bm info;
  int k;

  for(k=0; k<3; k++) info.c[k]=bmp[pos++];

  return info.i;
}

void BMP_draw(const char bmp[], int ix, int iy)
{
  int i=0, j=0, s=0, bmx=0, bmy=0, number, number2;
  int bfOffBits = bf4(bmp,10);//비트맵 데이터의 시작 위치
  int biWidth = bf4(bmp,18);    //480-1
  int biHeight = bf4(bmp,22);   //272-1

  while(1){

	  number = bmp[bfOffBits+i];
	      if(bmx>biWidth-1){bmx=0;bmy++;}
	      put_pixel(bmx+ix, biHeight-1-bmy+iy, colorpalette(bmp[bfOffBits+i], bmp));
	      bmx++;
      i+=1;
  }
}

void BMP_draw1(const char bmp[], int ix, int iy)
{
  int i=0, j=0, s=0, bmx=0, bmy=0, number, number2;
  int bfOffBits = bf4(bmp,10);//비트맵 데이터의 시작 위치
  int biWidth = bf4(bmp,18);    //480-1
  int biHeight = bf4(bmp,22);   //272-1

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
}

void bmp_main(void){

	Lcd_Init();

	while(1){
		//BMP_draw(dd, 0, 0);
	}

}

static int heart_buf[17] ={0,};
static int count; static int flag, sum;
int maxx, minn, s_max=0, s_min;
int first = 1;
int ADC_Val;
int re = 0;

void msecint(void)
{	// ADC

	sum = sum - heart_buf[flag];
	heart_buf[flag] = ADC_Val;
	sum = sum + heart_buf[flag];

	flag = (flag < 16) ? flag + 1 : 0;

		maxx = (sum > maxx) ? sum : maxx;
		minn = (sum < minn) ? sum : minn;


	if(count > 20){
		s_max = maxx;
		s_min = minn;

		maxx = sum;
		minn = sum;
		count = 0;
	}

	count++;

}

/*

#define HR_MAX 180
#define HR_MIN 80
#define T_MAX 60000/HR_MIN
#define T_MIN 60000/HR_MAX
*/

int adv;
int buf[17] = {0,};
int ready=0;
int nort[20];
int tm, diss; int sel = 0;
int peak_val;
int diff;
int s_flag = 1;
int ch, dis_flag ,distance;
int ds,t,t1,MAXX,cut_off;
unsigned int RR[] = {};
int k = 0;

void peak(void){
	t++; t1++;

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

	//t : timer cnt / t1 : flag 개념
	   if(diff>MAXX) {MAXX=diff; t1=0; } //MAXX 갱신
	   else {
	      if(t1>T_MIN) { // 노란색 떨어지는거임 - flag..
	         ds=t;


	         cut_off=(MAXX*2)/3;

	         t1=0; t=0;
	         MAXX=0;
	      }
	   }
	   if(MAXX< cut_off){      t1=0;} //측정하지말라고 함
	   if(t >T_MAX){ cut_off=0; MAXX=0; t1=0; t=0;}




	count++; tm++; sel++; diss++; distance++; re++;
}

int cutt; int fst_check = 1;


void heart_draw(void){

		if(s_max){
		if(gx+1 > MAX_X) {gx = 0; line(gx,0,gx,320,WHITE);}

		else {
		line(gx,0,gx,320,WHITE);
		//setcolor(YELLOW);
		//line(gx,320-(s_min/150), gx,320- (s_max/150));
		put_pixel(gx,320-diff/10,RED);
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

		if((sel > T_MIN) && (fst_check == 1)) { ch = cutt; distance = 0; cutt = 250; fst_check = 0; sel = 0; s_flag = 0;  dis_flag = 1; }
		if((sel > T_MIN) && (fst_check == 0)) { peak_val = ch; ch = 0; RR[k] = ds; k++; init_peak();}
	}



	if(s_max){
	/*//if(gx+1 > MAX_X) {gx = 0; line(gx,0,gx,320,WHITE);}

	else {
		//line(gx,0,gx,320,WHITE);
		//put_pixel(gx,ch, RED);

	}*/
	//gx = gx + 1;*/
	s_max = 0;

	}
}
//heart_beat
/*int cal_mean(int n){
	int sum, result = 0;
	int len = strlen(RR);
	for(int i = 0; i < len; i++){
		sum = sum + RR[i];
	}
	result = sum/(len+1);

	result = 60000/result;

	return result;
}

int cal_diff(int n){
	int DIFF = ADV[n] - ADV[n-1];

	return DIFF;
}

int cal_rmss(int n, int df){
	int rmss , sum;

	for(int i = 1; i < n; i++){
		df = cal_diff(i);
		sum = sum + df * df;
	}

	rmss = sqrt(sum/n);

	return rmss;
}

int cal_sdnn(int n, int mean){
	int sum, sdnn = 0;

	for(int i =1; i< n; i++){
		sum = sum + (mean - RR[i]) * (mean - RR[i]);
	}

	sdnn = sqrt(sum/(n+1));

	return sdnn;
}

void draw_rrv(int n){
	if(n > 0) {
		put_pixel(ADV[n],320 - ADV[n-1], RED);
	}
}

void heart_main(void){
	HAL_TIM_Base_Start_IT(&htim1);
	const unsigned char ssd1[] = "mean :";
	const unsigned char ssd2[] = "DF :";
	const unsigned char ssd3[] = "RMSS :";
	const unsigned char ssd4[] = "SDNN :";
	char val_rmss[10];
	char val_mean[10];
	char val_sdnn[10];
	char val_diff[10];
	int peak_val;
	Lcd_Init();
	init_peak();

	gx = 0; gy = 0;
	int len = strlen(ssd1);
	for(int i = 0; i< len; i++){
	putchar(ssd1[i]);
	}
	gx = 80;
	len = strlen(ssd2);
		for(int i = 0; i< len; i++){
		putchar(ssd2[i]);
	}
	gx = 0; gy = 25;
	len = strlen(ssd3);
		for(int i = 0; i< len; i++){
		putchar(ssd3[i]);
	}
	gx = 80;
	len = strlen(ssd4);
	for(int i = 0; i< len; i++){
	putchar(ssd4[i]);
	}

	int RMSS, SDNN, mean, DF;

	while(1){
		adv = ADV[re];

		if(80000 == re){
			mean = cal_mean(k);
			DF = cal_diff(re);
			RMSS = cal_rmss(re, DF);
			SDNN = cal_sdnn(k, mean);

			sprintf(val_mean,"%d",mean);
			len = strlen(val_mean);
			gx = 50; gy = 0;
			for(int i = 0; i< len; i++){
			putchar(val_mean[i]);
			}

			sprintf(val_diff,"%d",DF);
			len = strlen(val_diff);
			gx = 130;
			for(int i = 0; i< len; i++){
			putchar(val_diff[i]);
			}

			sprintf(val_rmss,"%d",RMSS);
			len = strlen(val_rmss);
			gx = 50; gy = 25;
			for(int i = 0; i< len; i++){
			putchar(val_rmss[i]);
			}

			sprintf(val_sdnn,"%d",SDNN);
			len = strlen(val_sdnn);
			gx = 130;
			for(int i = 0; i< len; i++){
			putchar(val_sdnn[i]);
			}

		}
	//if(s_max){
	draw_peak();
	//heart_draw();
	draw_rrv(re);
	//s_max = 0;
	//}
	if(re > 82000) {re = 83000;}
	}
}*/

const uint8_t crc7_table[256] =
{
    0x00, 0x09, 0x12, 0x1b, 0x24, 0x2d, 0x36, 0x3f,
    0x48, 0x41, 0x5a, 0x53, 0x6c, 0x65, 0x7e, 0x77,
    0x19, 0x10, 0x0b, 0x02, 0x3d, 0x34, 0x2f, 0x26,
    0x51, 0x58, 0x43, 0x4a, 0x75, 0x7c, 0x67, 0x6e,
    0x32, 0x3b, 0x20, 0x29, 0x16, 0x1f, 0x04, 0x0d,
    0x7a, 0x73, 0x68, 0x61, 0x5e, 0x57, 0x4c, 0x45,
    0x2b, 0x22, 0x39, 0x30, 0x0f, 0x06, 0x1d, 0x14,
    0x63, 0x6a, 0x71, 0x78, 0x47, 0x4e, 0x55, 0x5c,
    0x64, 0x6d, 0x76, 0x7f, 0x40, 0x49, 0x52, 0x5b,
    0x2c, 0x25, 0x3e, 0x37, 0x08, 0x01, 0x1a, 0x13,
    0x7d, 0x74, 0x6f, 0x66, 0x59, 0x50, 0x4b, 0x42,
    0x35, 0x3c, 0x27, 0x2e, 0x11, 0x18, 0x03, 0x0a,
    0x56, 0x5f, 0x44, 0x4d, 0x72, 0x7b, 0x60, 0x69,
    0x1e, 0x17, 0x0c, 0x05, 0x3a, 0x33, 0x28, 0x21,
    0x4f, 0x46, 0x5d, 0x54, 0x6b, 0x62, 0x79, 0x70,
    0x07, 0x0e, 0x15, 0x1c, 0x23, 0x2a, 0x31, 0x38,
    0x41, 0x48, 0x53, 0x5a, 0x65, 0x6c, 0x77, 0x7e,
    0x09, 0x00, 0x1b, 0x12, 0x2d, 0x24, 0x3f, 0x36,
    0x58, 0x51, 0x4a, 0x43, 0x7c, 0x75, 0x6e, 0x67,
    0x10, 0x19, 0x02, 0x0b, 0x34, 0x3d, 0x26, 0x2f,
    0x73, 0x7a, 0x61, 0x68, 0x57, 0x5e, 0x45, 0x4c,
    0x3b, 0x32, 0x29, 0x20, 0x1f, 0x16, 0x0d, 0x04,
    0x6a, 0x63, 0x78, 0x71, 0x4e, 0x47, 0x5c, 0x55,
    0x22, 0x2b, 0x30, 0x39, 0x06, 0x0f, 0x14, 0x1d,
    0x25, 0x2c, 0x37, 0x3e, 0x01, 0x08, 0x13, 0x1a,
    0x6d, 0x64, 0x7f, 0x76, 0x49, 0x40, 0x5b, 0x52,
    0x3c, 0x35, 0x2e, 0x27, 0x18, 0x11, 0x0a, 0x03,
    0x74, 0x7d, 0x66, 0x6f, 0x50, 0x59, 0x42, 0x4b,
    0x17, 0x1e, 0x05, 0x0c, 0x33, 0x3a, 0x21, 0x28,
    0x5f, 0x56, 0x4d, 0x44, 0x7b, 0x72, 0x69, 0x60,
    0x0e, 0x07, 0x1c, 0x15, 0x2a, 0x23, 0x38, 0x31,
    0x46, 0x4f, 0x54, 0x5d, 0x62, 0x6b, 0x70, 0x79
};

uint8_t crc7(uint8_t crc, const uint8_t *buf, size_t len)
{
    while (len--)
        crc = crc7_table[(crc << 1) ^ *buf++];

    return crc;
}

char str[20];
uint8_t outbuf[],inbuf[512];

static uint8_t spi_read_write(uint8_t out_data)
{	int k,in_data=0;
	for(k=0x80;k>0;k=k>>1)
	{	in_data = in_data << 1;
		sclk_low();
		if (k & out_data) do_high(); else do_low();
		sclk_high();
		if ( read_sd() ) in_data++;
	}
    return (in_data);
}

void spi_xfer(const uint8_t *outbuf, uint8_t *inbuf, uint16_t len)
{   uint8_t val;
    while (len--)
    {	val = spi_read_write(outbuf ? *outbuf++ : 0xFF);
        if (inbuf)  {*inbuf++ = val; }//printf("%08x\r\n",val);}
    }
}

#define SD_CMD(x) (0x40 | (x & 0x3f))
void enable_cs(void) {    ss_low(); }
void disable_cs(void) { ss_high(); }

static inline uint16_t swap16(uint16_t val){  return (((val & 0xFF00) >> 8) | ((val & 0x00FF) << 8));}
unsigned int swap32(uint32_t val)
{    return (((val & 0xFF000000) >> 24) |  ((val & 0x00FF0000) >> 8)  |
            ((val & 0x0000FF00) << 8)   |  ((val & 0x000000FF) << 24));
}
static void set_spi_mode(void) {   spi_xfer(NULL, NULL, 80); }
static uint8_t send_sd_cmd(uint8_t cmd, uint32_t arg)
{   uint8_t     spi_buf[8];
    uint8_t     r1 = 0;
    int         i;
    enable_cs();
    spi_buf[0] = 0xFF;
    spi_buf[1] = SD_CMD(cmd);
    spi_buf[2] = (arg >> 24) & 0xFF;
    spi_buf[3] = (arg >> 16) & 0xFF;
    spi_buf[4] = (arg >>  8) & 0xFF;
    spi_buf[5] = arg & 0xFF;
    spi_buf[6] = ((crc7(0, &spi_buf[1], 5) << 1) | 0x01) & 0xFF;
    spi_buf[7] = 0xFF;
    spi_xfer(spi_buf, NULL, 8);
    for (i=0; i<8; i++)
    {   spi_xfer(NULL, &r1, 1);
        if (!(r1 & 0x80)) break;
    }
    disable_cs();
    return r1;
}
static void sd_set_idle(void)
{   uint8_t r1;
    r1 = send_sd_cmd(0, 0);
    //printf("Set Idle(CMD0): %s\n", r1 == 0x01 ? "PASS" : "FAIL");
}
static void send_if_cond(void)
{   uint8_t     r1;
    uint32_t    data;
    r1 = send_sd_cmd(8, 0x1AA);
   // printf("SEND_IF_COND: ");
    if (r1 == 0x01)
    {   enable_cs();
        spi_xfer(NULL, (void*)&data, 4);
        disable_cs();
        //printf("%08X\n", swap32(data));
        return;
    }
   // printf("FAIL\n");
}
static void send_op_cond(void)
{   uint8_t     r1;
    int         timeout = 1000;
    sprintf(str,"SEND_OP_COND: ");   for(int i = 0; i<20; i++) //{putchar(str[i]);} gy++;
    while (1)
    {   r1 = send_sd_cmd(55, 0);
        if (r1 != 0x01)
        {   sprintf(str,"FAIL\n");    for(int i = 0; i<20; i++) //{putchar(str[i]);} gy++;
            return;
        }
        r1 = send_sd_cmd(41, 0x40000000);
        if (r1 == 0x00)
        {   sprintf(str,"PASS\n");   for(int i = 0; i<20; i++) //{putchar(str[i]);} gy++;
            return;
        }
        if (timeout-- <= 0)
        {   sprintf(str,"Time out\n");    for(int i = 0; i<20; i++) //{putchar(str[i]); } gy++;
            return;
        }
    }
}

static void sd_read_ocr(void)
{   uint8_t r1;
    uint32_t    data;
    r1 = send_sd_cmd(58, 0);
    printf("READ OCR: ");
    if (!(r1 & 0xFE))
    {   enable_cs();
        spi_xfer(NULL, (void*)&data, 4);
        disable_cs();
        printf("%08X\r\n", swap32(data));
        return;
    }
    printf(str,"FAIL\r\n");
}
void microsd_init(void)
{   disable_cs();
    HAL_Delay(100);
    set_spi_mode();
    sd_set_idle();
    send_if_cond();
    send_op_cond();
    sd_read_ocr();
}

int _read(int file, char *ptr, int len)
{
	if(HAL_UART_Receive(&huart2, (uint8_t *)ptr, len, 10) == HAL_OK)
		return len;
	return -1;
}

int _write(int file, char *ptr, int len)
{
	if(HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, 10) == HAL_OK)
		return len;
	return -1;
}

int __io_putchar(int ch)
{
	if(HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 10) != HAL_OK)
		return -1;
	return ch;
}

int __io_getchar(void)
{
	char data[4];
	uint8_t ch, len = 1;

	while(HAL_UART_Receive(&huart2, &ch, 1, 10) != HAL_OK)
	{
	}

	memset(data, 0x00, 4);
	switch(ch)
	{
		case '\r':
		case '\n':
			len = 2;
			sprintf(data, "\r\n");
			break;

		case '\b':
		case 0x7F:
			len = 3;
			sprintf(data, "\b \b");
			break;

		default:
			data[0] = ch;
			break;
	}
	HAL_UART_Transmit(&huart2, (uint8_t *)data, len, 10);
	return ch;
}

void rd_data(uint8_t *addr, uint16_t len){
	sclk_low();
	do_high();
	sclk_high();
	int dat = 0;
	int del;
	while(len--){
		for(int i = 0; i <8; i++){
			dat<<=1;
			sclk_low();
			if(read_sd()) dat++;
			sclk_high();
			del=50;
			while(del) del--;
		}
		if(flag){
			*addr = dat;
			addr++;
		}
		if(dat==0xfe) flag = 1;

		printf("%x\r\n",dat);
		dat = 0;
	}
}

static uint8_t in_data[512];
static int add_count = 0;

void spi_read(void){
	uint8_t r1;
	uint8_t st;
	uint8_t cr;

	printf("start : \r\n");

	enable_cs();

	r1 = send_sd_cmd(17, 0x10 + add_count);
	printf("r1 = %08x\r\n",r1);

	enable_cs();

	while(st != 0xfe){
		spi_xfer(NULL, &st, 1);
		printf("st = %08x\r\n",st);
	}

	spi_xfer(NULL, outbuf, 512);
	spi_xfer(NULL, &cr, 2);

	disable_cs();

	for(int i = 0; i<512; i=i+2){
	printf("outbuf[%d] = %08x   ",(512*add_count)+i,outbuf[(512*add_count)+i]);
	printf("outbuf[%d] = %08x\r\n",(512*add_count)+i+1,outbuf[(512*add_count)+i+1]);
	}
	printf("CRC = %08x\r\n", CRC);

	/*r1 = send_sd_cmd(12, 0);
	HAL_Delay(500);*/

}

void spi_write(int flag)
{
	uint8_t r1, r2;
	uint8_t st = 0xfe;
	uint8_t finish = 0xfd;

	printf("start : \r\n");

	enable_cs();

	r1 = send_sd_cmd(24, 0x10 + add_count);
	printf("r1 = %08x\r\n",r1);
	while(r1 != 0x00){

	}

	enable_cs();

	spi_xfer(&st, &r1, 1);
	spi_xfer(bmp3+flag, inbuf, 511);
	//spi_xfer(NULL,&r1,2);

	while((r2 & 0x1F) != 0x05){
			spi_xfer(NULL, &r2, 1);
			printf("r2 = %08x\r\n",r2);
		}

	while(r2 != 0x00){
			spi_xfer(NULL, &r2, 1);
	}

	disable_cs();

	r1 = send_sd_cmd(12, 0);
	HAL_Delay(500);
	r1 = send_sd_cmd(7, 0);
	HAL_Delay(500);
	r1 = send_sd_cmd(7, 0);
	HAL_Delay(500);
}

void sd_main(void)
{
	unsigned int bmp_flag;
	gx = 0; gy = 0;
	Lcd_Init();
	microsd_init();
	microsd_init();

	for(bmp_flag=0; bmp_flag<12200; bmp_flag += 511){
		spi_write(bmp_flag);
		spi_read();
		add_count++;
		HAL_Delay(500);
		/*gx += 256; gy = gy+8;
		if(gx > MAX_X) {gx = gx - MAX_X; gy++;}
		HAL_Delay(500);*/
	}


	while(1)
	{
		BMP_draw1(outbuf,gx,gy);
	}
}

FATFS fs;      /* Work area (filesystem object) for logical drives */

FATFS SDFatFs;       /* File system object for SD card logical drive */
FIL MyFile, MyFile1; /* File objects */
char SDPath[4];      /* SD card logical drive path */

void disk_main(void){

		Lcd_Init();
		microsd_init();
		microsd_init();

	    FIL fsrc;      // File objects
	    BYTE buffer[2048];    //File copy buffer
	    FRESULT fr;           //FatFs function common result code
	    UINT br, bw;          //File read/write count

	     //Give work areas to each logical drive
	    if(f_mount(&fs, "", 0) != FR_OK)
	    	printf("mount_error\r\n");
	    else printf("mount_success\r\n");

	    // Open source file on the drive 1
	    fr = f_open(&fsrc, "\a4.bmp", FA_READ);
	    if (fr != FR_OK) printf("open_error\r\n");
	    else printf("open_success\r\n");
	    printf("%d\r\n",fr);

	     //Copy source to destination
	    for (;;) {
	        fr = f_read(&fsrc, buffer, sizeof(buffer), &br);  //Read a chunk of data from the source file
	        if (fr != FR_OK) printf("read_error\r\n");
	        else printf("read_success\r\n");

	        if (br == 0) break;  //error or eof
	        //fr = f_write(&fdst, buffer, br, &bw);            //Write it to the destination file
	        if (bw < br) break;  //error or disk full
	    }

	     //Close open files
	    f_close(&fsrc);

	     //Unregister work area prior to discard it
	    //f_unmount("");
	    //f_unmount("1:");

	    return 0;

}
