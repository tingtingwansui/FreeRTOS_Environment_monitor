/**
 ****************************************************************************************************
 * @file        atk_fire.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2024-11-01
 * @brief       ���洫���� ��������
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


#ifndef __ATK_FIRE_H
#define __ATK_FIRE_H
#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* ���� ���� */

#define ATK_FIRE_DO_GPIO_PORT                  GPIOA
#define ATK_FIRE_DO_GPIO_PIN                   GPIO_PIN_6
#define ATK_FIRE_DO_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   

/******************************************************************************************/
/* ��DO���ź궨�� */

#define ATK_FIRE_DO         HAL_GPIO_ReadPin(ATK_FIRE_DO_GPIO_PORT, ATK_FIRE_DO_GPIO_PIN)  

/******************************************************************************************/
/* �ⲿ�ӿں���*/
void atk_fire_init(void);                   /* ��ʼ�� */
uint8_t atk_fire_get_val(void);             /* ��ȡ����ǿ�� */

#endif
