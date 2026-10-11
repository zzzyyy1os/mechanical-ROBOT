/**
 * @file TB6612.h
 * @brief TB6612FNG 电机驱动模块 (STM32 HAL, 多实例, 可移植)
 *
 * 直接复制 TB6612.c / TB6612.h 到工程即可使用。
 * GPIO 和定时器由 CubeMX 初始化，本模块只负责电机控制。
 *
 * 支持同一 MCU 上同时驱动多个 TB6612 (每个电机一个句柄)。
 *
 * 使用示例:
 *   // ---- CubeMX 初始化 TIM1 PWM (PE9 = TIM1_CH1) 后 ----
 *
 *   // 电机1: AIN1=PE8, AIN2=PE10, PWM=TIM1_CH1
 *   TB6612_HandleTypeDef motor1 = {
 *       .AIN1_Port = GPIOE, .AIN1_Pin = GPIO_PIN_8,
 *       .AIN2_Port = GPIOE, .AIN2_Pin = GPIO_PIN_10,
 *       .htim = &htim1, .Channel = TIM_CHANNEL_1,
 *   };
 *
 *   TB6612_Init(&motor1);
 *   TB6612_SetSpeed(&motor1, 80);    // 正转 80%
 *   TB6612_SetSpeed(&motor1, -80);   // 反转 80%
 *   TB6612_Stop(&motor1);            // 滑行停止
 *   TB6612_Brake(&motor1);           // 刹车制动
 */
#ifndef __TB6612_H
#define __TB6612_H

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TB6612 电机句柄 (每个电机实例一个)
 */
typedef struct {
    /* AIN1 引脚 (方向控制) */
    GPIO_TypeDef    *AIN1_Port;
    uint16_t         AIN1_Pin;

    /* AIN2 引脚 (方向控制) */
    GPIO_TypeDef    *AIN2_Port;
    uint16_t         AIN2_Pin;

    /* PWM 定时器句柄 (CubeMX 生成) */
    TIM_HandleTypeDef *htim;
    uint32_t           Channel;   // TIM_CHANNEL_1 / _2 / _3 / _4
} TB6612_HandleTypeDef;

/**
 * @brief  启动 PWM 输出 (GPIO/定时器由 CubeMX 初始化)
 * @param  htb: 电机句柄
 */
void TB6612_Init(TB6612_HandleTypeDef *htb);

/**
 * @brief  设置电机速度
 * @param  htb:   电机句柄
 * @param  speed: 速度值, 范围 -100 ~ +100
 *                正数正转, 负数反转, 0 停止
 */
void TB6612_SetSpeed(TB6612_HandleTypeDef *htb, int8_t speed);

/**
 * @brief  滑行停止 (AIN1=0, AIN2=0, 电机自由停转)
 * @param  htb: 电机句柄
 */
void TB6612_Stop(TB6612_HandleTypeDef *htb);

/**
 * @brief  刹车制动 (AIN1=1, AIN2=1, 电机立即停转)
 * @param  htb: 电机句柄
 */
void TB6612_Brake(TB6612_HandleTypeDef *htb);

#ifdef __cplusplus
}
#endif

#endif /* __TB6612_H */