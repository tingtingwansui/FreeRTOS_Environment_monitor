/**
 ****************************************************************************************************
 * @file        atk_mb026_uart.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2024-11-28
 * @brief       ATK-MB026ģ��UART�ӿ���������
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

#ifndef __ATK_MB026_UART_H
#define __ATK_MB026_UART_H

#include "./SYSTEM/sys/sys.h"

/* ���Ŷ��� */
#define ATK_MB026_UART_TX_GPIO_PORT           GPIOB
#define ATK_MB026_UART_TX_GPIO_PIN            GPIO_PIN_10
#define ATK_MB026_UART_TX_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0) /* PB��ʱ��ʹ�� */

#define ATK_MB026_UART_RX_GPIO_PORT           GPIOB
#define ATK_MB026_UART_RX_GPIO_PIN            GPIO_PIN_11
#define ATK_MB026_UART_RX_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0) /* PB��ʱ��ʹ�� */

#define ATK_MB026_TIM_INTERFACE               TIM2
#define ATK_MB026_TIM_IRQn                    TIM2_IRQn
#define ATK_MB026_TIM_IRQHandler              TIM2_IRQHandler
#define ATK_MB026_TIM_CLK_ENABLE()            do{ __HAL_RCC_TIM2_CLK_ENABLE();}while(0)
#define ATK_MB026_TIM_PRESCALER               7200

#define ATK_MB026_UART_INTERFACE              USART3
#define ATK_MB026_UART_IRQn                   USART3_IRQn
#define ATK_MB026_UART_IRQHandler             USART3_IRQHandler
#define ATK_MB026_UART_CLK_ENABLE()           do{ __HAL_RCC_USART3_CLK_ENABLE(); }while(0) /* USART3 ʱ��ʹ�� */

/* UART�շ������С */
#define ATK_MB026_UART_RX_BUF_SIZE            128
#define ATK_MB026_UART_TX_BUF_SIZE            128

/* �������� */
void atk_mb026_uart_printf(char *fmt, ...);       /* ATK-MB026 UART printf */
void atk_mb026_uart_rx_restart(void);             /* ATK-MB026 UART���¿�ʼ�������� */
uint8_t *atk_mb026_uart_rx_get_frame(void);       /* ��ȡATK-MB026 UART���յ���һ֡���� */
uint16_t atk_mb026_uart_rx_get_frame_len(void);   /* ��ȡATK-MB026 UART���յ���һ֡���ݵĳ��� */
void atk_mb026_uart_init(uint32_t baudrate);      /* ATK-MB026 UART��ʼ�� */

#endif
