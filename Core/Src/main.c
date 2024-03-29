/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "rtc.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "usbd_cdc_if.h"
#include "key.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint8_t aShow[100] = {0};
uint8_t usbcdcflag = 0;
unsigned short key_value;

extern uint32_t rtc_ResetFlag;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

static void RTC_CalendarShow(uint8_t *show);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
	uint32_t tick,tick1,tick2,tick3;
	uint8_t breathsw = 1;

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
  MX_RTC_Init();
	
	/* Turn on LED1: External reset occurred */
	if(rtc_ResetFlag != 1)	
	{
		for(uint8_t i=0;i<10;i++)
		{
			 HAL_GPIO_WritePin(C13_GPIO_Port,C13_Pin,GPIO_PIN_RESET);
			 HAL_Delay(100-1);
			 HAL_GPIO_WritePin(C13_GPIO_Port,C13_Pin,GPIO_PIN_SET);
			 HAL_Delay(100-1);
		}
		//HAL_Delay(1500-1);
	}
	
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
	  /* C13 呼吸灯测试 */
	  /* C13 Breathing Lamp test */
		static uint8_t pwmset;
		static uint16_t time;
		static uint8_t timeflag;
		static uint8_t timecount;
	  if((HAL_GetTick() - tick >= 1 ))
	  {
		   tick = HAL_GetTick();
			
		   if(breathsw == 1)
			{
				 /* 呼吸灯 */
				 /* Breathing Lamp */
				if(timeflag == 0)
				{
					time ++;
					if(time >= 1600) timeflag = 1;
				}
				else
				{
					time --;
					if(time == 0) timeflag = 0;
				}

				/* 占空比设置 */
				/* Duty Cycle Setting */
				pwmset = time/80;

				/* 20ms 脉宽 */
				/* 20ms Pulse Width */
				if(timecount > 20) timecount = 0;
				else timecount ++;

				if(timecount >= pwmset ) HAL_GPIO_WritePin(C13_GPIO_Port,C13_Pin,GPIO_PIN_SET);
				else HAL_GPIO_WritePin(C13_GPIO_Port,C13_Pin,GPIO_PIN_RESET);
			}
		}
	  
	  /* USB RTC ADC 测试 */
	  /* USB RTC ADC Test */
	  if(HAL_GetTick() - tick1 >= 1000)
	  {
		  tick1 = HAL_GetTick();
		  if(usbcdcflag == 1)
		  {
			  usbcdcflag = 0;
			  CDC_Transmit_FS(aShow,sprintf((char *)aShow, "\r\nSTM32F411CE WeAct CoreBoard\r\n  5V -> 3.3V LDO: AP7343D \r\n"));
				HAL_Delay(1);
				CDC_Transmit_FS(aShow,sprintf((char *)aShow, "KEY:\r\n  Short Press 短按\r\n  Double Press 双击\r\n  Long Press 长按\r\n"));
				
		  }
		  else
			  RTC_CalendarShow(aShow);
	  }
	  
	  /* 1ms 按键扫描 */
	  if(HAL_GetTick() - tick2 >= 1)
	  {
		  tick2 = HAL_GetTick();
		  key_check_all_loop_1ms();
	  }
	  
	  /* Key按键按下查询 */
	  if(HAL_GetTick() - tick3 >= 10)
	  {
		  tick3 = HAL_GetTick();
		  key_value = key_read_value();
		  
		  if(key_value == KEY0_UP_SHORT)
		  {
			  breathsw = 0;
			  HAL_GPIO_WritePin(C13_GPIO_Port,C13_Pin,HAL_GPIO_ReadPin(C13_GPIO_Port,C13_Pin)==GPIO_PIN_SET?GPIO_PIN_RESET:GPIO_PIN_SET);
				
			  CDC_Transmit_FS(aShow,sprintf((char *)aShow, "\r\nShort Press 短按\r\n"));
		  }
		  else if(key_value == KEY0_UP_DOUBLE)
		  {
			  breathsw = 0;
			  HAL_GPIO_WritePin(C13_GPIO_Port,C13_Pin,HAL_GPIO_ReadPin(C13_GPIO_Port,C13_Pin)==GPIO_PIN_SET?GPIO_PIN_RESET:GPIO_PIN_SET);
			  
			  CDC_Transmit_FS(aShow,sprintf((char *)aShow, "\r\nDouble Press 双击\r\n"));
		  }
		  else if(key_value == KEY0_LONG)
		  {
			  breathsw = 1;
			  CDC_Transmit_FS(aShow,sprintf((char *)aShow, "\r\nLong Press 长按\r\n"));
		  }
	  }
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

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_5);
}

/* USER CODE BEGIN 4 */

/**
  * @brief  Display the current time and date.
  * @param  show : pointer to buffer
  * @retval None
  */
static void RTC_CalendarShow(uint8_t *show)
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;
  int32_t size;
	
  /* 必须同时获取时间和日期 不然会导致下次RTC不能读取 */
  /* Both time and date must be obtained or RTC cannot be read next time */
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
  /* Display time Format : hh:mm:ss Display date Format : mm-dd-yy */
  size = sprintf((char *)show, "%02d:%02d:%02d,%02d月%02d日%2d年\r\n", stimestructureget.Hours,  
																					stimestructureget.Minutes,
																					stimestructureget.Seconds,
																					sdatestructureget.Month,
																					sdatestructureget.Date,
																					2000 + sdatestructureget.Year);
  CDC_Transmit_FS(show,size);
}

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
