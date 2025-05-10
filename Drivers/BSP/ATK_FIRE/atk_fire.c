/**
 ****************************************************************************************************
 * @file        atk_fire.c
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

#include "./BSP/ATK_FIRE/atk_fire.h"
#include "./BSP/ADC/adc.h"


/**
 * @brief       火焰传感器模块初始化函数
 * @param       无
 * @retval      无
 */
void atk_fire_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    ATK_FIRE_DO_GPIO_CLK_ENABLE();                              /* DO时钟使能 */

    gpio_init_struct.Pin = ATK_FIRE_DO_GPIO_PIN;                /* DO引脚 */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                    /* 输入 */
    gpio_init_struct.Pull = GPIO_NOPULL;                        /* 不上拉 不下拉 */
    HAL_GPIO_Init(ATK_FIRE_DO_GPIO_PORT, &gpio_init_struct);    /* DO引脚初始化 */

    adc_init();                                                 /* 初始化ADC */
}

/**
 * @brief       读取火焰传感器值
 * @param       无
 * @retval      0~100:0,最弱;100,最强
 */
uint8_t atk_fire_get_val(void)
{
    uint32_t temp_val = 0;
    temp_val = adc_get_result_average(ADC_ADCX_3_CHY, 10);  /* 读取平均值 */
    temp_val /= 40;

    if (temp_val > 100)temp_val = 100;

    return (uint8_t)(100 - temp_val);
}
