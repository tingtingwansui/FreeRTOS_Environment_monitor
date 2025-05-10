/**
 ****************************************************************************************************
 * @file        atk_fan.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2024-11-01
 * @brief       风扇模块 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 M48Z-M3最小系统板STM32F103版
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./BSP/ATK_FAN/atk_fan.h"
#include "./BSP/TIM/atim.h"


/**
 * @brief       风扇模块初始化函数
 * @param       无
 * @retval      无
 */
void atk_fan_init(void)
{
    atim_timx_cplm_pwm_init(50 - 1, 72 - 1);     /* 1Mhz的计数频率,20Khz的PWM */
    atk_fan_stop();
}

/**
 * @brief       电机停止
 * @param       无
 * @retval      无
 */
void atk_fan_stop(void)
{
    HAL_TIM_PWM_Stop(&g_timx_cplm_pwm_handle, TIM_CHANNEL_1);          /* 关闭主通道输出 */
    HAL_TIMEx_PWMN_Stop(&g_timx_cplm_pwm_handle, TIM_CHANNEL_1);       /* 关闭互补通道输出 */
}

/**
 * @brief       电机旋转方向设置
 * @param       para:方向 0正转，1反转
 * @note        以电机正面，顺时针方向旋转为正转
 * @retval      无
 */
void atk_fan_dir(uint8_t para)
{
    HAL_TIM_PWM_Stop(&g_timx_cplm_pwm_handle, TIM_CHANNEL_1);          /* 关闭主通道输出 */
    HAL_TIMEx_PWMN_Stop(&g_timx_cplm_pwm_handle, TIM_CHANNEL_1);       /* 关闭互补通道输出 */

    if (para == 0)                /* 正转 */
    {
        HAL_TIM_PWM_Start(&g_timx_cplm_pwm_handle, TIM_CHANNEL_1);     /* 开启主通道输出 */
    } 
    else if (para == 1)           /* 反转 */
    {
        HAL_TIMEx_PWMN_Start(&g_timx_cplm_pwm_handle, TIM_CHANNEL_1);  /* 开启互补通道输出 */
    }
}

/**
 * @brief       电机速度设置
 * @param       para:比较寄存器值
 * @retval      无
 */
void atk_fan_set_speed(uint16_t para)
{
    if (para < (__HAL_TIM_GetAutoreload(&g_timx_cplm_pwm_handle)))  /* 限速 */
    {  
        __HAL_TIM_SetCompare(&g_timx_cplm_pwm_handle, TIM_CHANNEL_1, para);
    }
}

/**
 * @brief       电机控制
 * @param       para: pwm比较值 ,正数电机为正转，负数为反转
 * @note        根据传入的参数控制电机的转向和速度
 * @retval      无
 */
void atk_fan_pwm_set(float para)
{
    int val = (int)para;

    if (val >= 0) 
    {
        atk_fan_dir(0);           /* 正转 */
        atk_fan_set_speed(val);
    } 
    else 
    {
        atk_fan_dir(1);           /* 反转 */
        atk_fan_set_speed(-val);
    }
}

