/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "i2c.h"
#include "TB6612.h"
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
/* USER CODE BEGIN Variables */
#include "tim.h"

/* 电机实例: AIN1=PE8, AIN2=PE10, PWM=TIM1_CH1 (PE9) */
TB6612_HandleTypeDef motor1 = {
    .AIN1_Port = GPIOE, .AIN1_Pin = GPIO_PIN_8,
    .AIN2_Port = GPIOE, .AIN2_Pin = GPIO_PIN_10,
    .htim      = &htim1,
    .Channel   = TIM_CHANNEL_1,
};
/* USER CODE END Variables */
/* Definitions for OLEDTask */
osThreadId_t OLEDTaskHandle;
const osThreadAttr_t OLEDTask_attributes = {
  .name = "OLEDTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for MOTORTask */
osThreadId_t MOTORTaskHandle;
const osThreadAttr_t MOTORTask_attributes = {
  .name = "MOTORTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartOLEDTask(void *argument);
void StartMOTORTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of OLEDTask */
  OLEDTaskHandle = osThreadNew(StartOLEDTask, NULL, &OLEDTask_attributes);

  /* creation of MOTORTask */
  MOTORTaskHandle = osThreadNew(StartMOTORTask, NULL, &MOTORTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartOLEDTask */
/**
  * @brief  Function implementing the OLEDTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartOLEDTask */
void StartOLEDTask(void *argument)
{
  /* USER CODE BEGIN StartOLEDTask */
  /* Initialize OLED display */
  OLED_Init();

  /* Test counter */
  uint32_t count = 0;

  /* Show startup message */
  OLED_Clear();
  OLED_ShowString(0, 0,  "SSD1306 OLED OK");
  OLED_ShowString(0, 16, "STM32F407 HAL");
  OLED_ShowString(0, 32, "I2C1 PB6/PB7");
  OLED_ShowString(0, 48, "FreeRTOS Ready!");
  OLED_Display();

  osDelay(2000);

  /* Infinite loop - update display every 500ms */
  for(;;)
  {
    OLED_Clear();

    /* Line 0: Title */
    OLED_ShowString(0, 0, "OLED Test");

    /* Line 1: Counter */
    OLED_ShowString(0, 16, "Count:");
    OLED_ShowNum(36, 16, count, 6);

    /* Line 2: Float test */
    OLED_ShowString(0, 32, "Float:");
    OLED_ShowFloat(36, 32, 3.14159f, 1, 3);

    /* Line 3: Status */
    OLED_ShowString(0, 48, "Running...");

    OLED_Display();

    count++;
    osDelay(500);
  }
  /* USER CODE END StartOLEDTask */
}

/* USER CODE BEGIN Header_StartMOTORTask */
/**
* @brief Function implementing the MOTORTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMOTORTask */
void StartMOTORTask(void *argument)
{
  /* USER CODE BEGIN StartMOTORTask */
  /* Initialize TB6612 motor driver */
  TB6612_Init(&motor1);

  /* Infinite loop - test motor speed control */
  for(;;)
  {
    /* 正转 */
    TB6612_SetSpeed(&motor1, 60);
    osDelay(2000);

    /* 加速 */
    TB6612_SetSpeed(&motor1, 80);
    osDelay(2000);

    /* 停止 */
    TB6612_SetSpeed(&motor1, 0);
    osDelay(1000);

    /* 反转 */
    TB6612_SetSpeed(&motor1, -60);
    osDelay(2000);

    /* 刹车 */
    TB6612_Brake(&motor1);
    osDelay(1000);
  }
  /* USER CODE END StartMOTORTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

