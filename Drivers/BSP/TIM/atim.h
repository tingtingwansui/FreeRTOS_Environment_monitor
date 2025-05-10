/**
 ****************************************************************************************************
 * @file        atim.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-01
 * @brief       �߼���ʱ�� ��������
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

#ifndef __ATIM_H
#define __ATIM_H

#include "./SYSTEM/sys/sys.h"


/* �߼���ʱ������Ƚ�ģʽ���� */
/* ���ͨ������ */
#define ATIM_TIMX_CPLM_CHY_GPIO_PORT            GPIOA
#define ATIM_TIMX_CPLM_CHY_GPIO_PIN             GPIO_PIN_8
#define ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)     /* PA��ʱ��ʹ�� */

/* �������ͨ������ */
#define ATIM_TIMX_CPLM_CHYN_GPIO_PORT           GPIOB
#define ATIM_TIMX_CPLM_CHYN_GPIO_PIN            GPIO_PIN_13
#define ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)     /* PB��ʱ��ʹ�� */

/* �������ʹ�õĶ�ʱ�� */
#define ATIM_TIMX_CPLM                          TIM1            
#define ATIM_TIMX_CPLM_CHY                      TIM_CHANNEL_1            
#define ATIM_TIMX_CPLM_CHY_CCRY                 ATIM_TIMX_CPLM->CCR1        
#define ATIM_TIMX_CPLM_CLK_ENABLE()             do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0)      /* TIM1 ʱ��ʹ�� */

extern TIM_HandleTypeDef g_timx_cplm_pwm_handle;                              /* ��ʱ��x��� */
/* �������� */
void atim_timx_cplm_pwm_init(uint16_t arr, uint16_t psc);                                       /* ��ʼ���߼���ʱ������PWM��� */
void atim_timx_cplm_pwm_set(uint16_t ccr, uint8_t dtg);                                         /* ���ø߼���ʱ������Ƚ�ֵ������ʱ�� */

#endif

















