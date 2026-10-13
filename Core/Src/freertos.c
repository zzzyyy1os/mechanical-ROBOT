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
#include "Emm_V5.h"
#include "usart.h"
#include "tim.h"
#include "pipeline.h"
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* 步进电机实例 (示例): 挂在 USART2 上, 总线地址 1 (拨码开关设定) */
Emm_V5_HandleTypeDef stepper1 = {
    .huart = &huart2,
    .addr  = 1,
};

/* TB6612 电机实例: AIN1=PE8, AIN2=PE10, PWM=TIM1_CH1 (PE9) */
TB6612_HandleTypeDef conveyor = {
    .AIN1_Port = GPIOE, .AIN1_Pin = GPIO_PIN_8,
    .AIN2_Port = GPIOE, .AIN2_Pin = GPIO_PIN_10,
    .htim      = &htim1,
    .Channel   = TIM_CHANNEL_1,
};
TB6612_HandleTypeDef Xcut = {
    .AIN1_Port = GPIOB, .AIN1_Pin = GPIO_PIN_0,
    .AIN2_Port = GPIOB, .AIN2_Pin = GPIO_PIN_1,
    .htim      = &htim1,
    .Channel   = TIM_CHANNEL_2,
};
TB6612_HandleTypeDef Ycut = {
    .AIN1_Port = GPIOB, .AIN1_Pin = GPIO_PIN_2,
    .AIN2_Port = GPIOB, .AIN2_Pin = GPIO_PIN_7,
    .htim      = &htim1,
    .Channel   = TIM_CHANNEL_3,
};

/* 要加更多步进电机就照上面再声明实例: 共用总线改 .addr, 换串口改 .huart */
// Emm_V5_HandleTypeDef stepper2 = { .huart = &huart2, .addr = 2 };
// Emm_V5_HandleTypeDef stepper3 = { .huart = &huart3, .addr = 1 };


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* 红外触发计数: StartqueueTask 写, OLED 显示 */
volatile uint32_t irTriggerCount = 0;

/* 摄像头采集标志: 主线程置1, MV线程采集完清零 */
volatile uint8_t  mvCaptureFlag  = 0;
uint8_t           mvBuf[64]      = {0};  /* MV线程持续接收缓冲 */
uint8_t           mvCaptureBuf[64] = {0};/* 采集时刻快照, 主线程读这个 */

/* XY切割完成标志: 切割任务完成后置1, 主线程读后复位 */
volatile uint8_t  xyCutDone      = 0;

/* USER CODE END Variables */
/* Definitions for OLEDTask */
osThreadId_t OLEDTaskHandle;
const osThreadAttr_t OLEDTask_attributes = {
  .name = "OLEDTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for queueTask */
osThreadId_t queueTaskHandle;
const osThreadAttr_t queueTask_attributes = {
  .name = "queueTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for MVTask */
osThreadId_t MVTaskHandle;
const osThreadAttr_t MVTask_attributes = {
  .name = "MVTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartOLEDTask(void *argument);
void StartqueueTask(void *argument);
void StartMVTask(void *argument);

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
  OLEDTaskHandle = osThreadNew(StartOLEDTask, NULL, &OLEDTask_attributes);//OLED

  /* creation of queueTask */
  queueTaskHandle = osThreadNew(StartqueueTask, NULL, &queueTask_attributes);//总线程

  /* creation of MVTask */
  MVTaskHandle = osThreadNew(StartMVTask, NULL, &MVTask_attributes);//摄像头

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

  /* Show startup message */
  OLED_Clear();

  osDelay(2000);

  /* Infinite loop - update display every 500ms */
  for(;;)
  {
    OLED_Clear();

    /* Line 0: Title */
    OLED_ShowString(0, 0, "OLED Test");

    /* Line 1: 工作时长 HH:MM:SS (补零格式 00:00:01) */
    uint32_t sec = osKernelGetTickCount() / configTICK_RATE_HZ;
    char timeStr[12];
    snprintf(timeStr, sizeof(timeStr), "%02lu:%02lu:%02lu",
             (unsigned long)(sec / 3600), (unsigned long)((sec / 60) % 60),
             (unsigned long)(sec % 60));
    OLED_ShowString(0, 16, "Time:");
    OLED_ShowString(32, 16, timeStr);

    /* Line 2: 红外中断计数 (第三行) */
    OLED_ShowString(0, 32, "IR:");
    OLED_ShowNum(24, 32, irTriggerCount, 6);

    /* Line 3: Status */
    OLED_ShowString(0, 48, "Running...");

    OLED_Display();

    osDelay(500);
  }
  /* USER CODE END StartOLEDTask */
}

/* USER CODE BEGIN Header_StartqueueTask */
/**
* @brief Function implementing the queueTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartqueueTask */
void StartqueueTask(void *argument)
{
  /* USER CODE BEGIN StartqueueTask */
  TB6612_Init(&conveyor);

  for(;;)
  {
    /* 空载状态: 传送带一直转 */
    Pipeline_SetState(MOTOR_NORMAL);
    TB6612_SetSpeed(&conveyor, 20);

    /* 等红外释放, 防止同一根重复触发 */
    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_RESET) { osDelay(10); }

    /* 等红外触发: PA1 低有效, 轮询+30ms 消抖 */
    for(;;) {
      while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET) { osDelay(10); }
      osDelay(30);
      if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_RESET) break;
    }
    irTriggerCount++;

    /* 触发: 传送带停, 处理阶段全程保持停止 */
    TB6612_Stop(&conveyor);
    Pipeline_SetState(INFRARED_DETECTED);

    /* 1) 摄像头数据读取: MV线程采集当前数据到 mvCaptureBuf */
    Pipeline_SetState(MV_WAIT);
    mvCaptureFlag = 1;
    while (mvCaptureFlag) { osDelay(1); }

    /* 2) XY切割 (下一步写); 没完成前传送带一直停着 */
    Pipeline_SetState(XY_CUTTING);
    while (!xyCutDone) { osDelay(10); }   /* 切割完成后置 xyCutDone=1 */
    xyCutDone = 0;

    /* 全部完成 -> 回循环顶, 传送带重新转动 */
  }
  /* USER CODE END StartqueueTask */
}

/* USER CODE BEGIN Header_StartMVTask */
/**
* @brief Function implementing the MVTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMVTask */
void StartMVTask(void *argument)
{
  /* USER CODE BEGIN StartMVTask */
  /* MV: 常驻收摄像头串口 (USART6: TX=PC6, RX=PC7) */
  for(;;)
  {
    /* TODO: 确定摄像头帧格式和接收长度, 目前按1字节占位 */
    HAL_UART_Receive(&huart6, mvBuf, 1, 50);

    if (mvCaptureFlag) {
      memcpy(mvCaptureBuf, mvBuf, sizeof(mvCaptureBuf));
      mvCaptureFlag = 0;  /* 通知主线程: 数据已就绪 */
    }
  }
  /* USER CODE END StartMVTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

