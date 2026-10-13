#include "pipeline.h"

/*
 * 状态机: 主线程(queueTask)顺序推进流水线状态,
 * OLED/MV 等线程用 Pipeline_GetState() 读当前状态。
 *
 * 一根流程: MOTOR_NORMAL --红外--> INFRARED_DETECTED --> MV_WAIT
 *           --摄像头--> XY_CUTTING --> MOTOR_PACKAGE --> MOTOR_NORMAL
 */

static volatile Pipeline_t s_state = MOTOR_NORMAL;

void Pipeline_SetState(Pipeline_t st)
{
    s_state = st;
}

Pipeline_t Pipeline_GetState(void)
{
    return s_state;
}
