/**
 ****************************************************************************************************
 * @file        atk_buzzer.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2024-11-01
 * @brief       蜂鸣器 驱动代码
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


#ifndef __ATK_BUZZER_H
#define __ATK_BUZZER_H
#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* 引脚 定义 */

#define ATK_BUZZER_GPIO_PORT                  GPIOB
#define ATK_BUZZER_GPIO_PIN                   GPIO_PIN_7
#define ATK_BUZZER_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)          

/******************************************************************************************/
/* BUZZER引脚 宏定义 */
#define ATK_BUZZER(x)   do{ x ? \
                            HAL_GPIO_WritePin(ATK_BUZZER_GPIO_PORT, ATK_BUZZER_GPIO_PIN, GPIO_PIN_SET) : \
                            HAL_GPIO_WritePin(ATK_BUZZER_GPIO_PORT, ATK_BUZZER_GPIO_PIN, GPIO_PIN_RESET); \
                        }while(0)          

/* BUZZER取反 宏定义 */
#define ATK_BUZZER_TOGGLE()   do{ HAL_GPIO_TogglePin(ATK_BUZZER_GPIO_PORT, ATK_BUZZER_GPIO_PIN); }while(0)  

/******************************************************************************************/
/* 外部接口函数*/
void atk_buzzer_init(void);                 /* 初始化 */
#endif
