/**
 ****************************************************************************************************
 * @file        atk_buzzer.c
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

#include "./BSP/ATK_BUZZER/atk_buzzer.h"


/**
 * @brief       蜂鸣器模块初始化函数
 * @param       无
 * @retval      无
 */
void atk_buzzer_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    ATK_BUZZER_GPIO_CLK_ENABLE();                               /* 时钟使能 */

    gpio_init_struct.Pin = ATK_BUZZER_GPIO_PIN;                 /* ATK_BUZZER引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLDOWN;                      /* 下拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
    HAL_GPIO_Init(ATK_BUZZER_GPIO_PORT, &gpio_init_struct);     /* 引脚初始化 */

    ATK_BUZZER(0);                                              /* 关闭 蜂鸣器 */
}


