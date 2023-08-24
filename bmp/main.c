/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>


#include "E16X24.h"
#include "a3.h"
#include "b3.h"
#include "ARROW.h"
#include "HYUNDAI.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define RED 0xf800
#define GREEN 0x07E0
#define BLUE 0x001F
#define BLACK 0x0000
#define WHITE 0xFFFF
#define Display_width 480
#define Display_height 272

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

LTDC_HandleTypeDef hltdc;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_LTDC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int forcolor=BLACK, bakcolor=WHITE;
int pr_size=0,dispcolor,maxx=Display_width,maxy=Display_height,gx=0,gy=0,fsize=0;



volatile unsigned short *Buffer = (unsigned short *)0x24000000;

void putPixel(int x, int y, int color)
{
  if(x>=0&&x<=479&&y>=0&&y<=271) Buffer[x+y*480] = color;
}

int pute16x24(int ch)
{
#define bitx  16
#define bity  24
#define maxfont  96
  
  unsigned int i,k;
  unsigned int comp;
  union inch
  {    
    short unsigned int i;
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
    putPixel(gx+i,gy+k,dispcolor);
    comp = comp >> 1;
    }
  }
  if (gx+bitx+bitx<maxx) { gx = gx + bitx;  }  else { gx=0; gy = gy + bity; }
  return(1);
}


int putchar(int ch)
{  
  pute16x24(ch);
  return ch;
}


unsigned long int bf4(const char bmp[], int pos)
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








int colorpalette(int position, const char bmp[])
{ 
  
  position = position*4+54;
  
  union palette
  {    
    unsigned int i;  
    char c[4];
  };
  
  union palette color;
  
  color.c[3] =bmp[position++];
  color.c[2] =bmp[position++];
  color.c[1] =bmp[position++];
  color.c[0] =bmp[position++];
  
  short rgbBlue = ((color.i & 0xff000000) >> 24);
  short rgbGreen = ((color.i & 0x00ff0000) >> 16);
  short rgbRed = ((color.i & 0x0000ff00) >> 8);
  
  rgbRed = rgbRed >> 3;
  rgbGreen = rgbGreen >> 2;
  rgbBlue = rgbBlue >> 3;
  
  int colord = ((rgbRed<<11)|(rgbGreen<<5)|(rgbBlue));
 

  return colord;
}


void BMP_draw(const char bmp[], int ix, int iy)
{
  int i=0, j=0, bmx=0, bmy=0, number, number2;
  int bfOffBits = bf4(bmp,10);//비트맵 데이터의 시작 위치
  int biWidth = bf4(bmp,18);    //480-1
  int biHeight = bf4(bmp,22);   //272-1
  
  while(1){
    number = bmp[bfOffBits+i];
    if(number == 0x00)
    {
      number2 = bmp[bfOffBits+i+1];
      if(number2 == 0x00){i+=2;}
      if(number2 == 0x01){i+=2;break;}
      if(number2>=3 && number2<=255)
      {
        for(j=0; j<number2; j++){
          if(bmx>biWidth-1){bmx=0;bmy++;}
          putPixel(bmx+ix, biHeight-1-bmy+iy, colorpalette( bmp[bfOffBits+i+2+j], bmp)  );
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
        putPixel(bmx+ix, biHeight-1-bmy+iy, colorpalette(bmp[bfOffBits+i+1], bmp));
        bmx++;        
      }
      i+=2;
    }
  }
}





/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  

  


  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_LTDC_Init();
  /* USER CODE BEGIN 2 */
  
  
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_SET);// LCD bright(PWM HIGH)
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_RESET); // LED on!
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_SET); // LCD on!
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_SET); // Watchdog on!
  


  

  for(int x=0; x<480; x++){
    for(int y=0; y<272; y++){
      putPixel(x, y, 0xffff);
    }
  }
  
  
 
  
  
  
  
 
  
  
  

   
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */


  
  

  while (1)
  {

    
 
    
    BMP_draw(a3, 0, 0);
    


    
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Supply configuration update enable 
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Macro to configure the PLL clock source 
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLL3.PLL3M = 1;
  PeriphClkInitStruct.PLL3.PLL3N = 6;
  PeriphClkInitStruct.PLL3.PLL3P = 2;
  PeriphClkInitStruct.PLL3.PLL3Q = 2;
  PeriphClkInitStruct.PLL3.PLL3R = 4;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_3;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief LTDC Initialization Function
  * @param None
  * @retval None
  */
static void MX_LTDC_Init(void)
{

  /* USER CODE BEGIN LTDC_Init 0 */

  /* USER CODE END LTDC_Init 0 */

  LTDC_LayerCfgTypeDef pLayerCfg = {0};

  /* USER CODE BEGIN LTDC_Init 1 */

  /* USER CODE END LTDC_Init 1 */
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 15;
  hltdc.Init.VerticalSync = 1;
  hltdc.Init.AccumulatedHBP = 31;
  hltdc.Init.AccumulatedVBP = 13;
  hltdc.Init.AccumulatedActiveW = 511;
  hltdc.Init.AccumulatedActiveH = 285;
  hltdc.Init.TotalWidth = 559;
  hltdc.Init.TotalHeigh = 289;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 480;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 272;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg.FBStartAdress = 0x24000000;
  pLayerCfg.ImageWidth = 480;
  pLayerCfg.ImageHeight = 272;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LTDC_Init 2 */

  /* USER CODE END LTDC_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin : PF7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : PE11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PD8 PD9 PD6 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
