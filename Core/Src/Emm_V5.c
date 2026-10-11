/**
 * @file Emm_V5.c
 * @brief Emm_V5.0 闭环步进电机裸帧驱动 (STM32 HAL, 可移植)
 *
 * 帧格式: [addr][func][data...][0x6B] (大端序, 0x6B为固定尾字节)
 * 发送: HAL_UART_Transmit (阻塞)
 */
#include "Emm_V5.h"

/* ===== 内部 UART 句柄 ===== */
static UART_HandleTypeDef *emmv5_huart = NULL;

/**
 * @brief  初始化 (设置 UART 句柄)
 * @param  huart: CubeMX 生成的 UART 句柄 (如 &huart2)
 */
void Emm_V5_Init(UART_HandleTypeDef *huart)
{
    emmv5_huart = huart;
}

/* ===== 内部发送 ===== */
static void Emm_V5_Send(uint8_t *cmd, uint16_t len)
{
    if (emmv5_huart != NULL) {
        HAL_UART_Transmit(emmv5_huart, cmd, len, 100);
    }
}

/* ===== 使能/停止 ===== */

void Emm_V5_Enable(uint8_t addr)
{
    uint8_t cmd[] = {addr, 0xF3, 0xAB, 0x01, 0x00, 0x6B};
    Emm_V5_Send(cmd, 6);
}

void Emm_V5_Disable(uint8_t addr)
{
    uint8_t cmd[] = {addr, 0xF3, 0xAB, 0x00, 0x00, 0x6B};
    Emm_V5_Send(cmd, 6);
}

void Emm_V5_Stop(uint8_t addr)
{
    uint8_t cmd[] = {addr, 0xFE, 0x98, 0x00, 0x6B};
    Emm_V5_Send(cmd, 5);
}

/* ===== 角度模式 (位置模式, 相对运动) ===== */

void Emm_V5_Pos_CW(uint8_t addr, uint16_t speed, uint8_t acc, uint32_t pulse)
{
    uint8_t cmd[] = {
        addr, 0xFD,
        0x00,                                   /* dir: 顺时针 */
        (uint8_t)(speed >> 8),                  /* vel_H */
        (uint8_t)(speed >> 0),                  /* vel_L */
        acc,                                    /* acc */
        (uint8_t)(pulse >> 24),                 /* clk bit24-31 */
        (uint8_t)(pulse >> 16),                 /* clk bit16-23 */
        (uint8_t)(pulse >> 8),                  /* clk bit8-15 */
        (uint8_t)(pulse >> 0),                  /* clk bit0-7 */
        0x00,                                   /* raF: 相对运动 */
        0x00,                                   /* snF */
        0x6B
    };
    Emm_V5_Send(cmd, 13);
}

void Emm_V5_Pos_CCW(uint8_t addr, uint16_t speed, uint8_t acc, uint32_t pulse)
{
    uint8_t cmd[] = {
        addr, 0xFD,
        0x01,                                   /* dir: 逆时针 */
        (uint8_t)(speed >> 8),
        (uint8_t)(speed >> 0),
        acc,
        (uint8_t)(pulse >> 24),
        (uint8_t)(pulse >> 16),
        (uint8_t)(pulse >> 8),
        (uint8_t)(pulse >> 0),
        0x00, 0x00, 0x6B
    };
    Emm_V5_Send(cmd, 13);
}

/* ===== 速度模式 (持续转动) ===== */

void Emm_V5_Vel_CW(uint8_t addr, uint16_t speed, uint8_t acc)
{
    uint8_t cmd[] = {
        addr, 0xF6,
        0x00,                                   /* dir: 顺时针 */
        (uint8_t)(speed >> 8),
        (uint8_t)(speed >> 0),
        acc,
        0x00,                                   /* snF */
        0x6B
    };
    Emm_V5_Send(cmd, 8);
}

void Emm_V5_Vel_CCW(uint8_t addr, uint16_t speed, uint8_t acc)
{
    uint8_t cmd[] = {
        addr, 0xF6,
        0x01,                                   /* dir: 逆时针 */
        (uint8_t)(speed >> 8),
        (uint8_t)(speed >> 0),
        acc,
        0x00,
        0x6B
    };
    Emm_V5_Send(cmd, 8);
}