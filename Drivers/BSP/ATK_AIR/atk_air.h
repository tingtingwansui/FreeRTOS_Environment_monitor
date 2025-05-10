/**
 ****************************************************************************************************
 * @file        atk_air.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2024-11-01
 * @brief       ��������������ģ�� ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� M48Z-M3��Сϵͳ��STM32F103��
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#ifndef __ATK_AIR_H
#define __ATK_AIR_H

#include "./SYSTEM/sys/sys.h"


#define RL      1        /* ���ݿ�������������ģ��ԭ��ͼ��֪��RL = 1k */ 
#define R0      34       /* MQ135�ڽྻ�����е���ֵ����ͬ���������嶼����ڲ��죬��Ԥ�Ⱥ����������ȡƽ��ֵ�õ� */
#define VC      5.0f     /* MQ135�����ѹ����5V */
#define VREF    3.3f     /* STM32��ADC�Ĳο���ѹ */
#define A       4.103    /* y = ax^b �� a */
#define B       -2.317   /* y = ax^b �� b */

/**************************************************************************************************/
/* ���� ���� */

#define ATK_AIR_DO_GPIO_PORT            GPIOB
#define ATK_AIR_DO_GPIO_PIN             GPIO_PIN_0
#define ATK_AIR_DO_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

/**************************************************************************************************/
/* ��DO���� �궨�� */

#define ATK_AIR_DO    HAL_GPIO_ReadPin(ATK_AIR_DO_GPIO_PORT, ATK_AIR_DO_GPIO_PIN)

/**************************************************************************************************/

void atk_air_init(void);
uint32_t atk_air_adc_converted_value(void);
float atk_air_get_ppm(void);

#endif

