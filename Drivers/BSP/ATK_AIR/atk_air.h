/**
 ****************************************************************************************************
 * @file        atk_air.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2024-11-01
 * @brief       空气质量传感器模块 驱动代码
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

#ifndef __ATK_AIR_H
#define __ATK_AIR_H

#include "./SYSTEM/sys/sys.h"


#define RL      1        /* 根据空气质量传感器模块原理图可知：RL = 1k */ 
#define R0      34       /* MQ135在洁净空气中的阻值，不同传感器个体都会存在差异，需预热后测量，这里取平均值得到 */
#define VC      5.0f     /* MQ135供电电压，接5V */
#define VREF    3.3f     /* STM32的ADC的参考电压 */
#define A       4.103    /* y = ax^b 的 a */
#define B       -2.317   /* y = ax^b 的 b */

/**************************************************************************************************/
/* 引脚 定义 */

#define ATK_AIR_DO_GPIO_PORT            GPIOB
#define ATK_AIR_DO_GPIO_PIN             GPIO_PIN_0
#define ATK_AIR_DO_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

/**************************************************************************************************/
/* 读DO引脚 宏定义 */

#define ATK_AIR_DO    HAL_GPIO_ReadPin(ATK_AIR_DO_GPIO_PORT, ATK_AIR_DO_GPIO_PIN)

/**************************************************************************************************/

void atk_air_init(void);
uint32_t atk_air_adc_converted_value(void);
float atk_air_get_ppm(void);

#endif

