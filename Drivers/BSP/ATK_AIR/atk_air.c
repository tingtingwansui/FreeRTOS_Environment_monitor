/**
 ****************************************************************************************************
 * @file        atk_air.c
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

#include "./BSP/ATK_AIR/atk_air.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/ADC/adc.h"
#include "math.h"


/**
 * @brief       空气质量传感器模块初始化函数
 * @param       无
 * @retval      无
 */
void atk_air_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    ATK_AIR_DO_GPIO_CLK_ENABLE();                                   /* DO时钟使能 */

    gpio_init_struct.Pin = ATK_AIR_DO_GPIO_PIN;                     /* DO引脚 */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                        /* 输入 */
    gpio_init_struct.Pull = GPIO_NOPULL;                            /* 无上下拉 */
    HAL_GPIO_Init(ATK_AIR_DO_GPIO_PORT, &gpio_init_struct);         /* DO引脚初始化 */

    adc_init();                                                     /* 初始化ADC */
}

/**
 * @brief       读取AO端电压转换值
 * @param       无
 * @retval      AO端电压转换值
 */
uint32_t atk_air_adc_converted_value(void)
{
    return adc_get_result_average(ADC_ADCX_2_CHY, 10);
}

/**
 * @brief  求ppm值函数
 * @param  无
 * @retval ppm
 * @note
 * 本函数根据MQ-135_规格书提供的传感器灵敏度特性曲线来拟合成幂函数
 * 根据Rs/R0推算ppm，所以拟合函数时，x轴为Rs/R0，y轴为ppm，推导的式子为：y = ax^b
 * 规格书上的曲线没有每个点的具体数值，所以只能大致估算，会存在误差，大家可以多次标定减少误差
 */
float atk_air_get_ppm(void)
{
    uint32_t adc_converted_value;       /* AO端电压转换值 */
    float vrl;                          /* AO端的电压（传感器串联的负载电阻RL上的电压） */
    float rs;                           /* 当前传感器的电阻 */
    float ppm;                          /* 可燃气体平均浓度 */
    float rs0;                          /* 传感器电阻比（在测试环境传感器的阻值比上在洁净空气环境传感器的阻值） */
    
    adc_converted_value = atk_air_adc_converted_value();
    //printf("AO端电压转换值为：%d\r\n", adc_converted_value);
    
    vrl = (float)adc_converted_value / 4096.f * VREF;  /* 计算AO端的电压 */
    //printf("AO端电压（vrl）为：%.3fV\r\n", vrl);
    
    rs = (float)(VC - vrl) * RL / vrl;                 /* 计算传感器的阻值 */
    //printf("当前MQ135传感器电阻值为：%.1fkΩ\r\n", rs);
    
    rs0 = rs / R0;  /* Rs / R0 */
    //printf("Rs/R0 = %0.3f\r\n", rs0);

    /* y = ax^b； 其中x为Rs/R0，a和b的取值根据数据手册图表自行拟合成幂函数 */
    ppm =  A * pow(rs0, B);

    return ppm;
}
