/**
 * @file Emm_V5.h
 * @brief Emm_V5.0 闭环步进电机裸帧驱动 (STM32 HAL, 多实例, 可移植)
 *
 * 直接复制 Emm_V5.c / Emm_V5.h 到工程即可使用。
 * UART 由 CubeMX 初始化，本模块只负责帧封装和发送。
 *
 * 支持同一 MCU 上同时驱动多个步进电机 (每个电机一个句柄)。
 * 多个电机可共用一条 UART 总线 (不同 addr)，也可各占一条 UART。
 *
 * 使用示例:
 *   // ---- CubeMX 初始化 UART 后 ----
 *
 *   // 电机1: 总线地址 1，挂在 UART2 上
 *   Emm_V5_HandleTypeDef motor1 = { .huart = &huart2, .addr = 1 };
 *
 *   // 电机2: 总线地址 2，同一条总线
 *   Emm_V5_HandleTypeDef motor2 = { .huart = &huart2, .addr = 2 };
 *
 *   Emm_V5_Enable(&motor1);                      // 使能
 *   Emm_V5_Pos_CW(&motor1, 500, 10, 3200);       // 顺时针转3200脉冲(1圈)
 *   Emm_V5_Vel_CW(&motor2, 500, 10);             // 顺时针500RPM持续转
 *   Emm_V5_Stop(&motor2);                        // 停止
 *   Emm_V5_Disable(&motor1);                     // 关闭使能
 *
 * 注意: 多任务同时通过同一条 UART 发帧时，需自行加互斥 (如 mutex)。
 */
#ifndef __EMM_V5_H
#define __EMM_V5_H

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 步进电机句柄 (每个电机实例一个)
 */
typedef struct {
    /* UART 句柄 (CubeMX 生成，多个电机可共用同一条总线) */
    UART_HandleTypeDef *huart;
    /* 驱动器总线地址 */
    uint8_t             addr;
} Emm_V5_HandleTypeDef;

/* ===== 使能/停止 ===== */

/**
 * @brief  使能电机 (锁轴)
 * @param  hemm: 电机句柄
 */
void Emm_V5_Enable(Emm_V5_HandleTypeDef *hemm);

/**
 * @brief  关闭使能 (松轴)
 * @param  hemm: 电机句柄
 */
void Emm_V5_Disable(Emm_V5_HandleTypeDef *hemm);

/**
 * @brief  立即停止
 * @param  hemm: 电机句柄
 */
void Emm_V5_Stop(Emm_V5_HandleTypeDef *hemm);

/* ===== 角度模式 (位置模式，相对运动) ===== */

/**
 * @brief  顺时针转指定脉冲数
 * @param  hemm:  电机句柄
 * @param  speed: 转速 (RPM), 0~5000
 * @param  acc:   加速度, 0~255
 * @param  pulse: 脉冲数 (3200脉冲=1圈@16细分)
 */
void Emm_V5_Pos_CW(Emm_V5_HandleTypeDef *hemm, uint16_t speed, uint8_t acc, uint32_t pulse);

/**
 * @brief  逆时针转指定脉冲数
 */
void Emm_V5_Pos_CCW(Emm_V5_HandleTypeDef *hemm, uint16_t speed, uint8_t acc, uint32_t pulse);

/* ===== 速度模式 (持续转动) ===== */

/**
 * @brief  顺时针持续转动
 * @param  hemm:  电机句柄
 * @param  speed: 转速 (RPM), 0~5000
 * @param  acc:   加速度, 0~255
 */
void Emm_V5_Vel_CW(Emm_V5_HandleTypeDef *hemm, uint16_t speed, uint8_t acc);

/**
 * @brief  逆时针持续转动
 */
void Emm_V5_Vel_CCW(Emm_V5_HandleTypeDef *hemm, uint16_t speed, uint8_t acc);

#ifdef __cplusplus
}
#endif

#endif /* __EMM_V5_H */