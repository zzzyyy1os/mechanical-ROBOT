#ifndef __PIPELINE_H
#define __PIPELINE_H

// 流水线状态 (主线程 queueTask 推进, 其他线程可读)
typedef enum {
    MOTOR_NORMAL,      // 空载: 传送带转动, 等红外
    INFRARED_DETECTED, // 红外检测到物体, 传送带停
    MV_WAIT,           // 等摄像头信号
    XY_CUTTING,        // XY切割
    MOTOR_PACKAGE,     // 打包
} Pipeline_t;

void       Pipeline_SetState(Pipeline_t st);  /* 主线程: 切状态 */
Pipeline_t Pipeline_GetState(void);           /* 任意线程: 读状态 */

#endif
