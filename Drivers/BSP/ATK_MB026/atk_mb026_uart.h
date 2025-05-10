/**
 ****************************************************************************************************
 * @file        atk_mb026_uart.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2024-11-28
 * @brief       ATK-MB026模块UART接口驱动代码
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

#ifndef __ATK_MB026_UART_H
#define __ATK_MB026_UART_H

#include "./SYSTEM/sys/sys.h"

/* 引脚定义 */
#define ATK_MB026_UART_TX_GPIO_PORT           GPIOB
#define ATK_MB026_UART_TX_GPIO_PIN            GPIO_PIN_10
#define ATK_MB026_UART_TX_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0) /* PB口时钟使能 */

#define ATK_MB026_UART_RX_GPIO_PORT           GPIOB
#define ATK_MB026_UART_RX_GPIO_PIN            GPIO_PIN_11
#define ATK_MB026_UART_RX_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0) /* PB口时钟使能 */

#define ATK_MB026_TIM_INTERFACE               TIM2
#define ATK_MB026_TIM_IRQn                    TIM2_IRQn
#define ATK_MB026_TIM_IRQHandler              TIM2_IRQHandler
#define ATK_MB026_TIM_CLK_ENABLE()            do{ __HAL_RCC_TIM2_CLK_ENABLE();}while(0)
#define ATK_MB026_TIM_PRESCALER               7200

#define ATK_MB026_UART_INTERFACE              USART3
#define ATK_MB026_UART_IRQn                   USART3_IRQn
#define ATK_MB026_UART_IRQHandler             USART3_IRQHandler
#define ATK_MB026_UART_CLK_ENABLE()           do{ __HAL_RCC_USART3_CLK_ENABLE(); }while(0) /* USART3 时钟使能 */

/* UART收发缓冲大小 */
#define ATK_MB026_UART_RX_BUF_SIZE            128
#define ATK_MB026_UART_TX_BUF_SIZE            128

/* 操作函数 */
void atk_mb026_uart_printf(char *fmt, ...);       /* ATK-MB026 UART printf */
void atk_mb026_uart_rx_restart(void);             /* ATK-MB026 UART重新开始接收数据 */
uint8_t *atk_mb026_uart_rx_get_frame(void);       /* 获取ATK-MB026 UART接收到的一帧数据 */
uint16_t atk_mb026_uart_rx_get_frame_len(void);   /* 获取ATK-MB026 UART接收到的一帧数据的长度 */
void atk_mb026_uart_init(uint32_t baudrate);      /* ATK-MB026 UART初始化 */

#endif
