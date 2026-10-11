/**
 * @file Emm_V5.h
 * @brief Emm_V5.0 闭环步进电机裸帧驱动 (STM32 HAL, 可移植)
 *
 * 直接复制 Emm_V5.c / Emm_V5.h 到工程即可使用。
 * UART 由 CubeMX 初始化，本模块只负责帧封装和发送。
 *
 * 使用示例:
 *   Emm_V5_Enable(1);                          // 使能
 *   Emm_V5_Pos_CW(1, 500, 10, 3200);           // 顺时针转3200脉冲(1圈)
 *   Emm_V5_Vel_CW(1, 500, 10);                 // 顺时针500RPM持续转
 *   Emm_V5_Stop(1);                            // 停止
 *   Emm_V5_Disable(1);                         // 关闭使能
 */
#ifndef __EMM_V5_H
#define __EMM_V5_H

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ===== 初始化 ===== */

/**
 * @brief  初始化 (绑定 UART 句柄)
 * @param  huart: CubeMX 生成的 UART 句柄 (如 &huart2)
 */
void Emm_V5_Init(UART_HandleTypeDef *huart);

/* ===== 使能/停止 ===== */

/**
 * @brief  使能电机 (锁轴)
 * @param  addr: 驱动器地址
 */
void Emm_V5_Enable(uint8_t addr);

/**
 * @brief  关闭使能 (松轴)
 * @param  addr: 驱动器地址
 */
void Emm_V5_Disable(uint8_t addr);

/**
 * @brief  立即停止
 * @param  addr: 驱动器地址
 */
void Emm_V5_Stop(uint8_t addr);

/* ===== 角度模式 (位置模式, 相对运动) ===== */

/**
 * @brief  顺时针转指定脉冲数
 * @param  addr:  驱动器地址
 * @param  speed: 转速 (RPM), 0~5000
 * @param  acc:   加速度, 0~255
 * @param  pulse: 脉冲数 (3200脉冲=1圈@16细分)
 */
void Emm_V5_Pos_CW(uint8_t addr, uint16_t speed, uint8_t acc, uint32_t pulse);

/**
 * @brief  逆时针转指定脉冲数
 */
void Emm_V5_Pos_CCW(uint8_t addr, uint16_t speed, uint8_t acc, uint32_t pulse);

/* ===== 速度模式 (持续转动) ===== */

/**
 * @brief  顺时针持续转动
 * @param  addr:  驱动器地址
 * @param  speed: 转速 (RPM), 0~5000
 * @param  acc:   加速度, 0~255
 */
void Emm_V5_Vel_CW(uint8_t addr, uint16_t speed, uint8_t acc);

/**
 * @brief  逆时针持续转动
 */
void Emm_V5_Vel_CCW(uint8_t addr, uint16_t speed, uint8_t acc);

#ifdef __cplusplus
}
#endif

#endif /* __EMM_V5_H */