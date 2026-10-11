/**
 * @file TB6612.c
 * @brief TB6612FNG 电机驱动模块 (STM32 HAL, 多实例, 可移植)
 *
 * 直接复制 TB6612.c / TB6612.h 到工程即可使用。
 * GPIO 和定时器由 CubeMX 初始化，本模块只负责电机控制。
 * 支持同一 MCU 上同时驱动多个 TB6612。
 */
#include "TB6612.h"

/**
 * @brief  启动 PWM 输出
 * @param  htb: 电机句柄
 */
void TB6612_Init(TB6612_HandleTypeDef *htb)
{
    HAL_TIM_PWM_Start(htb->htim, htb->Channel);
}

/**
 * @brief  设置电机速度
 * @param  htb:   电机句柄
 * @param  speed: -100 ~ +100, 正数正转, 负数反转
 */
void TB6612_SetSpeed(TB6612_HandleTypeDef *htb, int8_t speed)
{
    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(htb->htim);
    uint32_t compare;

    if (speed >= 0)
    {
        /* 正转: AIN1=1, AIN2=0 */
        HAL_GPIO_WritePin(htb->AIN1_Port, htb->AIN1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(htb->AIN2_Port, htb->AIN2_Pin, GPIO_PIN_RESET);

        if (speed > 100) speed = 100;
        compare = ((uint32_t)speed * arr) / 100;
    }
    else
    {
        /* 反转: AIN1=0, AIN2=1 */
        HAL_GPIO_WritePin(htb->AIN1_Port, htb->AIN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(htb->AIN2_Port, htb->AIN2_Pin, GPIO_PIN_SET);

        if (speed < -100) speed = -100;
        compare = ((uint32_t)(-speed) * arr) / 100;
    }

    __HAL_TIM_SET_COMPARE(htb->htim, htb->Channel, compare);
}

/**
 * @brief  滑行停止 (AIN1=0, AIN2=0)
 */
void TB6612_Stop(TB6612_HandleTypeDef *htb)
{
    HAL_GPIO_WritePin(htb->AIN1_Port, htb->AIN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(htb->AIN2_Port, htb->AIN2_Pin, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(htb->htim, htb->Channel, 0);
}

/**
 * @brief  刹车制动 (AIN1=1, AIN2=1)
 */
void TB6612_Brake(TB6612_HandleTypeDef *htb)
{
    HAL_GPIO_WritePin(htb->AIN1_Port, htb->AIN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(htb->AIN2_Port, htb->AIN2_Pin, GPIO_PIN_SET);
    __HAL_TIM_SET_COMPARE(htb->htim, htb->Channel, 0);
}