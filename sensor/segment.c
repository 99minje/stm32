/*
#include "main.h"
#include "math.h"
#include "stdlib.h"
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern DAC_HandleTypeDef hdac1;

void data_high(void) {int delay = 10; Data_GPIO_Port->BSRR=Data_Pin; while(delay--); }
//#define data_high() Data_GPIO_Port->BSRR=Data_Pin
#define data_low() Data_GPIO_Port->BRR=Data_Pin

#define read_data() Data_GPIO_Port->IDR & Data_Pin

#define shift_high() SC_GPIO_Port->BSRR=SC_Pin
#define shift_low() SC_GPIO_Port->BRR=SC_Pin

#define latch_high() LC_GPIO_Port->BSRR=LC_Pin
#define latch_low() LC_GPIO_Port->BRR=LC_Pin

#define s0_high() s0_GPIO_Port->BSRR=s0_Pin
#define s0_low() s0_GPIO_Port->BRR=s0_Pin

#define s1_high() s1_GPIO_Port->BSRR=s1_Pin
#define s1_low() s1_GPIO_Port->BRR=s1_Pin

#define s2_high() s2_GPIO_Port->BSRR=s2_Pin
#define s2_low() s2_GPIO_Port->BRR=s2_Pin

#define s3_high() s3_GPIO_Port->BSRR=s3_Pin
#define s3_low() s3_GPIO_Port->BRR=s3_Pin

#define trig_high() TRIG_GPIO_Port->BSRR=TRIG_Pin
#define trig_low() TRIG_GPIO_Port->BRR=TRIG_Pin

#define trig2_high() TRIG2_GPIO_Port->BSRR=TRIG2_Pin
#define trig2_low() TRIG2_GPIO_Port->BRR=TRIG2_Pin

#define read_echo() ECHO_GPIO_Port->IDR & ECHO_Pin

#define read_echo2() ECHO2_GPIO_Port->IDR & ECHO2_Pin

unsigned int db[16]={0x7f,0x00,0x67,0x0,0x5f,0,0x7c,0x0,0x39,0x00,0x5e,0x0,0x79,0,0x71,0x0};
const unsigned int font[16]={0x3f,0x06, 0x5b, 0x4f , 0x66, 0x6d, 0x7d, 0x27,0x7f,0x67,0x5f,0x7c,0x39,0x5e,0x79,0x71};
unsigned int black = 0x00ff;
static unsigned int rd[4];
unsigned int init_format[8] = {0x0900, 0x0a04, 0x0b07, 0x0c01, 0x0f00, 0x0f00, 0x0f00, 0x0f00};
unsigned int del[8] = {0x0100, 0x0200, 0x0300, 0x0400, 0x0500, 0x0600, 0x0700 , 0x0800};

int msec,sec=0,min=0;

uint32_t ADC_Val=0;
unsigned int low = 0;

int l = 0;
int j = 0;

const char efn[] = {
           0x00,0x00,0x00,0x00,0x00,  0x00,0x00,0x4f,0x00,0x00,   // 1
           0x00,0x07,0x00,0x07,0x00,  0x14,0x7f,0x14,0x7f,0x14,   // 2
           0x24,0x2a,0x7f,0x2a,0x12,  0x23,0x13,0x08,0x64,0x62,   // 3
           0x36,0x49,0x55,0x22,0x50,  0x00,0x05,0x03,0x00,0x00,   // 4
           0x00,0x1c,0x22,0x41,0x00,  0x00,0x41,0x22,0x1c,0x00,   // 5
           0x14,0x08,0x3e,0x08,0x14,  0x08,0x08,0x3e,0x08,0x08,   // 6
           0x00,0x50,0x30,0x00,0x00,  0x08,0x08,0x08,0x08,0x08,   // 7
           0x00,0x60,0x60,0x00,0x00,  0x20,0x10,0x08,0x04,0x02,   // 8
           0x3e,0x51,0x49,0x45,0x3e,  0x00,0x42,0x7f,0x40,0x00,   // 9
           0x42,0x61,0x51,0x49,0x46,  0x21,0x41,0x45,0x4b,0x31,   // 10
           0x18,0x14,0x12,0x7f,0x10,  0x27,0x45,0x45,0x45,0x39,   // 11
           0x3c,0x4a,0x49,0x49,0x30,  0x01,0x71,0x09,0x05,0x03,   // 12
           0x36,0x49,0x49,0x49,0x36,  0x06,0x49,0x49,0x29,0x1e,   // 13
           0x00,0x00,0x36,0x36,0x00,  0x00,0x00,0x56,0x36,0x00,   // 14
           0x08,0x14,0x22,0x41,0x00,  0x14,0x14,0x14,0x14,0x14,   // 15
           0x00,0x41,0x22,0x14,0x08,  0x02,0x01,0x51,0x09,0x06,   // 16
           0x32,0x49,0x79,0x41,0x3e,  0x7e,0x11,0x11,0x11,0x7e,   // 17
           0x7f,0x49,0x49,0x49,0x36,  0x3e,0x41,0x41,0x41,0x22,   // 18
           0x7f,0x41,0x41,0x22,0x1c,  0x7f,0x49,0x49,0x49,0x41,   // 19
           0x7f,0x09,0x09,0x09,0x01,  0x3e,0x41,0x49,0x49,0x7a,   // 20
           0x7f,0x08,0x08,0x08,0x7f,  0x00,0x41,0x7f,0x41,0x00,   // 21
           0x20,0x40,0x41,0x3f,0x01,  0x7f,0x08,0x14,0x22,0x41,   // 22
           0x7f,0x40,0x40,0x40,0x40,  0x7f,0x02,0x0c,0x02,0x7f,   // 23
           0x7f,0x04,0x08,0x10,0x7f,  0x3e,0x41,0x41,0x41,0x3e,   // 24
           0x7f,0x09,0x09,0x09,0x06,  0x3e,0x41,0x51,0x21,0x5e,   // 25
           0x7f,0x09,0x19,0x29,0x46,  0x46,0x49,0x49,0x49,0x31,   // 26
           0x01,0x01,0x7f,0x01,0x01,  0x3f,0x40,0x40,0x40,0x3f,   // 27
           0x1f,0x20,0x40,0x20,0x1f,  0x3f,0x40,0x38,0x40,0x3f,   // 28
           0x63,0x14,0x08,0x14,0x63,  0x07,0x08,0x70,0x08,0x07,   // 29
           0x61,0x51,0x49,0x45,0x43,  0x00,0x7f,0x41,0x41,0x00,   // 30
           0x02,0x04,0x08,0x10,0x20,  0x00,0x41,0x41,0x7f,0x00,   // 31
           0x04,0x02,0x01,0x02,0x04,  0x40,0x40,0x40,0x40,0x40,   // 32
           0x01,0x02,0x04,0x00,0x00,  0x20,0x54,0x54,0x54,0x78,   // 33
           0x7f,0x48,0x44,0x44,0x38,  0x38,0x44,0x44,0x44,0x20,   // 34
           0x38,0x44,0x44,0x48,0x7f,  0x38,0x54,0x54,0x54,0x18,   // 35
           0x08,0x7e,0x09,0x01,0x02,  0x06,0x49,0x49,0x49,0x3f,   // 36
           0x7f,0x08,0x04,0x04,0x78,  0x00,0x44,0x7d,0x40,0x00,   // 37
           0x20,0x40,0x44,0x3d,0x00,  0x7f,0x10,0x28,0x44,0x00,   // 38
           0x00,0x41,0x7f,0x40,0x00,  0x7c,0x04,0x18,0x04,0x78,   // 39
           0x7c,0x08,0x04,0x04,0x78,  0x38,0x44,0x44,0x44,0x38,   // 40
           0x7c,0x14,0x14,0x14,0x08,  0x08,0x14,0x14,0x18,0x7c,   // 41
           0x7c,0x08,0x04,0x04,0x08,  0x48,0x54,0x54,0x54,0x20,   // 42
           0x04,0x3f,0x44,0x40,0x20,  0x3c,0x40,0x40,0x20,0x7c,   // 43
           0x1c,0x20,0x40,0x20,0x1c,  0x3c,0x40,0x30,0x40,0x3c,   // 44
           0x44,0x28,0x10,0x28,0x44,  0x0c,0x50,0x50,0x50,0x3c,   // 45
           0x44,0x64,0x54,0x4c,0x44,  0x00,0x08,0x36,0x41,0x00,   // 46
           0x00,0x00,0x7f,0x00,0x00,  0x00,0x41,0x36,0x08,0x00,   // 47
           0x08,0x04,0x08,0x10,0x08,
          };

void init(void){

	for(int j = 0; j<8; j++){
	latch_low();
	for(int i = 0 ; i<16; i++){
		shift_low();
		if(init_format[j] & (0x8000 >> (i))) { data_high();}	else{data_low();}
		shift_high();
	}
	latch_high();

	}
}

void zero(void){
	for(int j = 0; j<8; j++){
		latch_low();
		for(int z = 0; z<4; z++){
			for(int i = 0 ; i<16; i++){
				shift_low();
				if(del[j] & (0x8000 >> (i))) { data_high();}	else{data_low();}
				shift_high();
			}
		}
		latch_high();
		}
}

unsigned int DB[64] = { 0x0800,0x0800,0x0800,0x0800,0x0800,0x0800,0x0800,0x0800,
						0x0700,0x0700,0x0700,0x0700,0x0700,0x0700,0x0700,0x0700,
						0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,
						0x0500,0x0500,0x0500,0x0500,0x0500,0x0500,0x0500,0x0500,
						0x0400,0x0400,0x0400,0x0400,0x0400,0x0400,0x0400,0x0400,
						0x0300,0x0300,0x0300,0x0300,0x0300,0x0300,0x0300,0x0300,
						0x0200,0x0200,0x0200,0x0200,0x0200,0x0200,0x0200,0x0200,
						0x0100,0x0100,0x0100,0x0100,0x0100,0x0100,0x0100,0x0100};

char wt[6];

void set_wt(int char_num){

	int start = (char_num - 64)*5;

	for(int i = 0; i < 5; i++){
		wt[i] = efn[start + i];
	}
	wt[5] = 0x00;
}

void char_wt(int ord, int sh){

	for(int x =0; x < 6; x++){
		for(int y =0; y<8; y++){
			if(wt[x] & (1 << y)) db_wt((x+(ord*6)+sh),y);
		}
	}
}

void db_zero(void){

	for(int y = 0; y< 8; y++){
		for(int x = 0; x < 32; x++){
			db_del(x,y);
		}
	}
}

void db_wt(int Xa, int Y){

	int X = Xa % 64;
	int k = Y*8 + X/8;
	int j = X % 8;
	DB[k] = DB[k] | (1 << j);

}

void db_del(int Xa, int Y){

	int X = Xa % 64;
	int k = Y*8 + X/8;
	int j = X % 8;
	DB[k] = DB[k] & ~(1 << j);

}

void dot_on(void){

	for(int a = 0; a < 8; a++){
	latch_low();
	for(int z = 0; z<4; z++){
	for(int i = 0 ; i<16; i++){
			shift_low();
			if(DB[abs(3 - z+(a*8))] & (0x8000 >> (i))) { data_high();}	else{data_low();}
			shift_high();
		}
	}
	latch_high();
}}

int sht = 0;

void matrix_main(void){

		init();
		zero();
		//HAL_TIM_Base_Start_IT(&htim1);
		while(1)
		{
			db_zero();
			set_wt(104);
			char_wt(0, sht);
			set_wt(101);
			char_wt(1, sht);
			set_wt(108);
			char_wt(2, sht);
			set_wt(112);
			char_wt(3, sht);

			set_wt(64);
			char_wt(4, sht);

			set_wt(112);
			char_wt(5, sht);
			set_wt(108);
			char_wt(6, sht);
			set_wt(122);
			char_wt(7, sht);
			set_wt(65);
			char_wt(8, sht);

			dot_on();
			HAL_Delay(50);
			sht++;
			if(sht == 64) sht = 0;
		}

}

void seg_off(void)
{
	unsigned int off;


	off = black;
	if((sec % 10) == i){
		latch_low();
				shift_low();
				if(off & (1 << i)) { data_high();}	else{data_low();}
				shift_high();
		latch_high();
	}

}

void disp_seg(void)
{
	const unsigned int address[]={0x0800,0x0400, 0x0200, 0x0100 , 0x8000, 0x4000, 0x2000, 0x1000};
		unsigned int data;

		if (msec<999) msec++;
		else
		{	msec=0;
			if (sec<58) sec++;
			else { sec=0; min++; }

		}

		i = (i<7) ? i+1 : 0;
		data = address[i] | db[i];
		latch_low();
		for(j=0; j<16; j++){
			shift_low();
			if(data & (1 << j)) { data_high();}	else{data_low();}
			shift_high();
		}
		latch_high();
}

void dig4L(unsigned int k)
{	k = k % 10000;
	db[0] =font[ k /1000 ];
	db[1] =font[ ( k % 1000 )/100 ];
	db[2] =font[ ( k % 100 ) /10 ];
	db[3] =font[ k % 10 ];

}

void dig4R(unsigned int k)
{	k = k % 10000;
	db[4] =font[ k /1000 ];
	db[5] =font[ ( k % 1000 )/100 ];
	db[6] =font[ ( k % 100 ) /10 ];
	db[7] =font[ k % 10 ];
}

void dig8(unsigned int k)
{	dig4L(k/10000);
	dig4R(k%10000);
}

void seg_main(void)
{

	int old = 0;
	unsigned int k=0;
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_PWM_Start_IT(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	while(1)
	{	k++;
		dig8(k);

		if (old != sec)
		{
			old=sec;
			TIM2->CCR1 = 100 * (sec % 10);
			//TIM1->CCR1 = 100 * (sec % 10);


			TIM1->CCR1 = 0;


		}
	}
}

void seg8(unsigned int data)
{
	unsigned int j;

	for(j=0; j<8; j++){
		shift_low();
		if(data & (1 << j)) { data_high();}	else{data_low();}
		shift_high();
	}
}
void seg_comm(unsigned int data)
{
	latch_low();
	seg8(data);
	latch_high();
}
void seg_disp()
{
	seg_comm(0x40);	//Wr, Auto Inc
	latch_low();
	seg8(0xC0);
	for (int j = 0;j<16;j++) seg8(db[j]);
	latch_high();

}

int data_read(){

	int data;
	latch_low();
	seg8(0x42);
	data_high();

	for(int z = 0; z < 4; z++){
		data = 0;

	for(int x= 0; x<8; x++){
		shift_low();
		data = data << 1;
		shift_high();
		if(read_data()) data = data + 1;

	}
	rd[z] = data;
	}

	   latch_high();
	   data=0;
	   if(rd[0]&0x80) data=0x80;
	   if(rd[0]&0x08) data |=0x08;
	   if(rd[1]&0x80) data |= 0x40;
	   if(rd[1]&0x8) data|=0x4;
	   if(rd[2]&0x80) data|=0x20;
	   if(rd[2]&0x8) data|=0x2;
	   if(rd[3]&0x80) data|=0x10;
	   if(rd[3]&0x8) data|=1;

	   return(data);

}

#define pi 3.14
int flag;
void keyboard2(int val1){

		   if(val1&0x80) {
			   HAL_TIM_Base_Start_IT(&htim2);
			   TIM2->ARR = 1223;

			   	   	   }
		   else if(val1&0x08) {
			   HAL_TIM_Base_Start_IT(&htim2);
			   TIM2->ARR = 864;
		   }
		   else if(val1&0x80) {
			   HAL_TIM_Base_Start_IT(&htim2);
			   TIM2->ARR = 1089;

		   }
		   else if(val1&0x8) {
			   HAL_TIM_Base_Start_IT(&htim2);
			   TIM2->ARR = 816;

		   }
		   else if(val1&0x80) { HAL_TIM_Base_Start_IT(&htim2); TIM2->ARR = 970;

		   }
		   else if(val1&0x8) { HAL_TIM_Base_Start_IT(&htim2); TIM2->ARR = 727;

		   }
		   else if(val1&0x80) { HAL_TIM_Base_Start_IT(&htim2); TIM2->ARR = 916;

		   }
		   else if(val1&0x8) { HAL_TIM_Base_Start_IT(&htim2); TIM2->ARR = 648;

		   }
		   else HAL_TIM_Base_Stop_IT(&htim2);
}

void keyboard3(int val1){

	   if(val1&0x80) {
		   HAL_TIM_Base_Start_IT(&htim3);
		   TIM3->ARR = 1223;

		   	   	   }
	   else if(val1&0x08) {
		   HAL_TIM_Base_Start_IT(&htim3);
		   TIM3->ARR = 864;
	   }
	   else if(val1&0x80) {
		   HAL_TIM_Base_Start_IT(&htim3);
		   TIM3->ARR = 1089;

	   }
	   else if(val1&0x8) {
		   HAL_TIM_Base_Start_IT(&htim3);
		   TIM3->ARR = 816;

	   }
	   else if(val1&0x80) { HAL_TIM_Base_Start_IT(&htim3); TIM3->ARR = 970;

	   }
	   else if(val1&0x8) { HAL_TIM_Base_Start_IT(&htim3); TIM3->ARR = 727;

	   }
	   else if(val1&0x80) { HAL_TIM_Base_Start_IT(&htim3); TIM3->ARR = 916;

	   }
	   else if(val1&0x8) { HAL_TIM_Base_Start_IT(&htim3); TIM3->ARR = 648;

	   }
	   else HAL_TIM_Base_Stop_IT(&htim3);
}

unsigned short int sinT[100];
void sin_(){
   for(int j=0; j<100; j++){
      sinT[j]=1000*(1+sin(pi*j/50));
   }
}

unsigned short int sin2 = 0;

void dac_set2(void){
	static int i=0;

	   //if(rd[0] == 0x80 || rd[0] == 0x08 || rd[1] == 0x40 || rd[1] == 0x04 || rd[2] == 0x20 || rd[2] == 0x02 || rd[3] == 0x10 || rd[3] == 0x01) sin2 = 0;

	   if(i<99) i++;
	   else i=0;
	   HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1,DAC_ALIGN_12B_L, sin2 + sinT[i]);
}

void dac_set3(void){
	static int u=0;

	if(u<99) u++;
	else u=0;
	sin2 = sinT[u];

	//if(rd[0] == 0x80 || rd[0] == 0x08 || rd[1] == 0x40 || rd[1] == 0x04 || rd[2] == 0x20 || rd[2] == 0x02 || rd[3] == 0x10 || rd[3] == 0x01) sin2 =  0;

}


void piano_main(void){

		int key;

		HAL_TIM_Base_Start_IT(&htim2);
		HAL_TIM_Base_Start_IT(&htim3);
		HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
		sin_();

		while(1)
		{
			int first, seco = 0;
			seg_comm(0x8F);
			key = data_read();

			db[0] = font[key>>4];
			db[2] = font[key&0x0F];
			seg_disp();

			for(int i =0; i< 32; i++){
				if(key & (0x01 << i)) {
					if(!first) first = 0x01 << i;
					else seco = 0x01 << i;
				}
			}

			keyboard2(first);
			keyboard3(seco);
		}

}


void motor(void){

	static uint32_t position = 0;

	if(position < low) position++;
	else if(position > low) position--;

	dig4L(position);

	switch(position % 8){
	case 0: s0_high(); s1_low(); s2_low(); s3_low(); break;
	case 1: s0_high(); s1_high(); s2_low(); s3_low(); break;
	case 2: s0_low(); s1_high(); s2_low(); s3_low(); break;
	case 3: s0_low(); s1_high(); s2_high(); s3_low(); break;
	case 4: s0_low(); s1_low(); s2_high(); s3_low(); break;
	case 5: s0_low(); s1_low(); s2_high(); s3_high(); break;
	case 6: s0_low(); s1_low(); s2_low(); s3_high(); break;
	case 7: s0_high(); s1_low(); s2_low(); s3_high(); break;
	}
}

unsigned int LPF(unsigned int low , int tau, uint32_t adc) {

	low = (tau*low + adc) / (tau + 1);

	return low;
}

void ADC_main(void){

	uint32_t ADC_Val=0;
	unsigned int low = 0;//init low
	int tau = 12;

	HAL_TIM_Base_Start_IT(&htim1);
	while(!(HAL_ADCEx_Calibration_Start(&hadc1)==HAL_OK));

	while (1)
	  {
	    HAL_ADC_Start(&hadc1);
	    ADC_Val=HAL_ADC_GetValue(&hadc1);

	    dig4L(ADC_Val);

	    low = LPF(low, tau, ADC_Val);

	    dig4R(low);

	  }

}

void motor_main(void){

	int tau = 20;
	s0_low(); s1_low(); s2_low(); s3_low();
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_Base_Start_IT(&htim3);
	while(!(HAL_ADCEx_Calibration_Start(&hadc1)==HAL_OK));

	while (1)
	  {

		HAL_ADC_Start(&hadc1);
	    ADC_Val=HAL_ADC_GetValue(&hadc1);

	    low = LPF(low, tau, ADC_Val);
	    dig4R(low);

	  }

}

unsigned int MIN1(unsigned int value[]){

	unsigned int min = 0;

	for(int x = 0 ; x < 10; x++){
		if(value[i] < value[i+1]) min = value[i];
		else min = value[i+1];
	}

	return min;
}

unsigned int MIN2(unsigned int value[]){

	unsigned int min = 0;

	for(int x = 0 ; x < 10; x++){
		if(value[i] < value[i+1]) min = value[i];
		else min = value[i+1];
	}

	return min;
}

void dis_main(void){

		unsigned int mode1, mode2, distance1, distance2, count1, count2, pt1, pt2 = 0;
		unsigned int value1[10];
		unsigned int value2[10];

		HAL_TIM_Base_Start_IT(&htim1);
		HAL_TIM_Base_Start(&htim6);
		//HAL_TIM_Base_Start(&htim7);

		while (1)
		  {

			switch(mode1){
				case 0: TIM6 ->CNT = 0;
						trig_high();
						mode1 = 1;
						break;
				case 1: if(TIM6 ->CNT >= 10) {
							trig_low();
							mode1 = 2;
				}
							break;
				case 2: if(read_echo()) {
							mode1 = 3;
							TIM6 ->CNT = 0;
				}
						break;

				case 3: if((read_echo()) == 0){
							distance1 = (TIM6 ->CNT * 170) / 1000;
							//dig4R(distance1);
							count1 = TIM6->CNT;
							for(int x= 0; TIM6->CNT< count1 + 11; x++){
								value1[x] = distance1;
							}
							mode1 = 4;

				}
							break;

							case 4: TIM6 ->CNT = 0;
									trig2_high();
									mode1 = 5;
									break;
							case 5: if(TIM6 ->CNT >= 10) {
										trig2_low();
										mode1 = 6;
							}
										break;
							case 6: if(read_echo2()) {
										mode1 = 7;
										TIM6 ->CNT = 0;
							}
									break;

							case 7: if((read_echo2()) == 0){
										distance2 = (TIM6 ->CNT * 170) / 1000;
										//dig4L(distance2);
										count2 = TIM6->CNT;
										for(int x= 0; TIM6->CNT< count2 + 11; x++){
										value2[x] = distance2;
										}
										mode1 = 8;
										pt2++;
							}
										break;
			}

				if(TIM6 ->CNT > 50000) {mode1 = 0;
					if(pt2 > 10) {dig4R(distance1);
								  dig4L(distance2);
								  pt2 = 0;}
				}

		  }
}
*/
