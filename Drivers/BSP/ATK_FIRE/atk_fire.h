/**
 ****************************************************************************************************
 * @file        atk_fire.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2024-11-01
 * @brief       火焰传感器 驱动代码
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


#ifndef __ATK_FIRE_H
#define __ATK_FIRE_H
#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* 引脚 定义 */

#define ATK_FIRE_DO_GPIO_PORT                  GPIOA
#define ATK_FIRE_DO_GPIO_PIN                   GPIO_PIN_6
#define ATK_FIRE_DO_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   

/******************************************************************************************/
/* 读DO引脚宏定义 */

#define ATK_FIRE_DO         HAL_GPIO_ReadPin(ATK_FIRE_DO_GPIO_PORT, ATK_FIRE_DO_GPIO_PIN)  

/******************************************************************************************/
/* 外部接口函数*/
void atk_fire_init(void);                   /* 初始化 */
uint8_t atk_fire_get_val(void);             /* 读取火焰强度 */

#endif
