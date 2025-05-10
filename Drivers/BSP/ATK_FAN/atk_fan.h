/**
 ****************************************************************************************************
 * @file        atk_fan.h
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


#ifndef __ATK_FAN_H
#define __ATK_FAN_H
#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* �ⲿ�ӿں���*/
void atk_fan_init(void);                    /* ��ʼ�� */
void atk_fan_set_speed(uint16_t pwm_val);   /* ���ٺ��� */
void atk_fan_dir(uint8_t para);
void atk_fan_stop(void);
void atk_fan_pwm_set(float para);

#endif
