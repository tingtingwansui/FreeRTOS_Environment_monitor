/**
 ****************************************************************************************************
 * @file        atk_fan.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2024-11-01
 * @brief       ����ģ�� ��������
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

#include "./BSP/ATK_FAN/atk_fan.h"
#include "./BSP/TIM/atim.h"


/**
 * @brief       ����ģ���ʼ������
 * @param       ��
 * @retval      ��
 */
void atk_fan_init(void)
{
    atim_timx_cplm_pwm_init(50 - 1, 72 - 1);     /* 1Mhz�ļ���Ƶ��,20Khz��PWM */
    atk_fan_stop();
}

/**
 * @brief       ���ֹͣ
 * @param       ��
 * @retval      ��
 */
void atk_fan_stop(void)
{
    HAL_TIM_PWM_Stop(&g_timx_cplm_pwm_handle, TIM_CHANNEL_1);          /* �ر���ͨ����� */
    HAL_TIMEx_PWMN_Stop(&g_timx_cplm_pwm_handle, TIM_CHANNEL_1);       /* �رջ���ͨ����� */
}

/**
 * @brief       �����ת��������
 * @param       para:���� 0��ת��1��ת
 * @note        �Ե�����棬˳ʱ�뷽����תΪ��ת
 * @retval      ��
 */
void atk_fan_dir(uint8_t para)
{
    HAL_TIM_PWM_Stop(&g_timx_cplm_pwm_handle, TIM_CHANNEL_1);          /* �ر���ͨ����� */
    HAL_TIMEx_PWMN_Stop(&g_timx_cplm_pwm_handle, TIM_CHANNEL_1);       /* �رջ���ͨ����� */

    if (para == 0)                /* ��ת */
    {
        HAL_TIM_PWM_Start(&g_timx_cplm_pwm_handle, TIM_CHANNEL_1);     /* ������ͨ����� */
    } 
    else if (para == 1)           /* ��ת */
    {
        HAL_TIMEx_PWMN_Start(&g_timx_cplm_pwm_handle, TIM_CHANNEL_1);  /* ��������ͨ����� */
    }
}

/**
 * @brief       ����ٶ�����
 * @param       para:�ȽϼĴ���ֵ
 * @retval      ��
 */
void atk_fan_set_speed(uint16_t para)
{
    if (para < (__HAL_TIM_GetAutoreload(&g_timx_cplm_pwm_handle)))  /* ���� */
    {  
        __HAL_TIM_SetCompare(&g_timx_cplm_pwm_handle, TIM_CHANNEL_1, para);
    }
}

/**
 * @brief       �������
 * @param       para: pwm�Ƚ�ֵ ,�������Ϊ��ת������Ϊ��ת
 * @note        ���ݴ���Ĳ������Ƶ����ת����ٶ�
 * @retval      ��
 */
void atk_fan_pwm_set(float para)
{
    int val = (int)para;

    if (val >= 0) 
    {
        atk_fan_dir(0);           /* ��ת */
        atk_fan_set_speed(val);
    } 
    else 
    {
        atk_fan_dir(1);           /* ��ת */
        atk_fan_set_speed(-val);
    }
}

