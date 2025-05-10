/**
 ****************************************************************************************************
 * @file        atk_mb026_uart.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2024-11-28
 * @brief       ATK_MB026ģ��UART�ӿ���������
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

#include "./BSP/ATK_MB026/atk_mb026_uart.h"
#include "./SYSTEM/usart/usart.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static TIM_HandleTypeDef g_tim_handle;                      /* ATK_MB026 Timer */
static UART_HandleTypeDef g_uart_handle;                    /* ATK-MB026 UART */
static struct
{
    uint8_t buf[ATK_MB026_UART_RX_BUF_SIZE];              /* ֡���ջ��� */
    struct
    {
        uint16_t len    : 15;                               /* ֡���ճ��ȣ�sta[14:0] */
        uint16_t finsh  : 1;                                /* ֡������ɱ�־��sta[15] */
    } sta;                                                  /* ֡״̬��Ϣ */
} g_uart_rx_frame = {0};                                    /* ATK-MB026 UART����֡������Ϣ�ṹ�� */
static uint8_t g_uart_tx_buf[ATK_MB026_UART_TX_BUF_SIZE]; /* ATK-MB026 UART���ͻ��� */

/**
 * @brief       ATK-MB026 UART printf
 * @param       fmt: ����ӡ������
 * @retval      ��
 */
void atk_mb026_uart_printf(char *fmt, ...)
{
    va_list ap;
    uint16_t len;
    
    va_start(ap, fmt);
    vsprintf((char *)g_uart_tx_buf, fmt, ap);
    va_end(ap);
    
    len = strlen((const char *)g_uart_tx_buf);
    HAL_UART_Transmit(&g_uart_handle, g_uart_tx_buf, len, HAL_MAX_DELAY);
}

/**
 * @brief       ATK-MB026 UART���¿�ʼ��������
 * @param       ��
 * @retval      ��
 */
void atk_mb026_uart_rx_restart(void)
{
    g_uart_rx_frame.sta.len     = 0;
    g_uart_rx_frame.sta.finsh   = 0;
}

/**
 * @brief       ��ȡATK-MB026 UART���յ���һ֡����
 * @param       ��
 * @retval      NULL: δ���յ�һ֡����
 *              ����: ���յ���һ֡����
 */
uint8_t *atk_mb026_uart_rx_get_frame(void)
{
    if (g_uart_rx_frame.sta.finsh == 1)
    {
        g_uart_rx_frame.buf[g_uart_rx_frame.sta.len] = '\0';
        return g_uart_rx_frame.buf;
    }
    else
    {
        return NULL;
    }
}

/**
 * @brief       ��ȡATK-MB026 UART���յ���һ֡���ݵĳ���
 * @param       ��
 * @retval      0   : δ���յ�һ֡����
 *              ����: ���յ���һ֡���ݵĳ���
 */
uint16_t atk_mb026_uart_rx_get_frame_len(void)
{
    if (g_uart_rx_frame.sta.finsh == 1)
    {
        return g_uart_rx_frame.sta.len;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief       ATK-MB026 UART��ʼ��
 * @param       baudrate: UARTͨѶ������
 * @retval      ��
 */
void atk_mb026_uart_init(uint32_t baudrate)
{
    g_uart_handle.Instance          = ATK_MB026_UART_INTERFACE;     /* ATK-MB026 UART */
    g_uart_handle.Init.BaudRate     = baudrate;                     /* ������ */
    g_uart_handle.Init.WordLength   = UART_WORDLENGTH_8B;           /* ����λ */
    g_uart_handle.Init.StopBits     = UART_STOPBITS_1;              /* ֹͣλ */
    g_uart_handle.Init.Parity       = UART_PARITY_NONE;             /* У��λ */
    g_uart_handle.Init.Mode         = UART_MODE_TX_RX;              /* �շ�ģʽ */
    g_uart_handle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;          /* ��Ӳ������ */
    g_uart_handle.Init.OverSampling = UART_OVERSAMPLING_16;         /* ������ */
    HAL_UART_Init(&g_uart_handle);                                  /* ʹ��ATK-MB026 UART
                                                                     * HAL_UART_Init()����ú���HAL_UART_MspInit()
                                                                     * �ú����������ļ�usart.c��
                                                                     */
    g_tim_handle.Instance           = ATK_MB026_TIM_INTERFACE;      /* ATK_MB026 Timer */
    g_tim_handle.Init.Prescaler     = ATK_MB026_TIM_PRESCALER - 1;  /* Ԥ��Ƶϵ�� */
    g_tim_handle.Init.Period        = 100 - 1;                      /* �Զ���װ��ֵ */
    HAL_TIM_Base_Init(&g_tim_handle);                               /* ��ʼ��Timer����UART���ճ�ʱ��� */
}

/**
 * @brief       ATK_MB026 Timer��ʼ��MSP�ص�����
 * @param       htim: Timer���ָ��
 * @retval      ��
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == ATK_MB026_TIM_INTERFACE)
    {
        ATK_MB026_TIM_CLK_ENABLE();                         /* ʹ��Timerʱ�� */
        
        HAL_NVIC_SetPriority(ATK_MB026_TIM_IRQn, 0, 0);     /* ��ռ���ȼ�0�������ȼ�0 */
        HAL_NVIC_EnableIRQ(ATK_MB026_TIM_IRQn);             /* ʹ��Timer�ж� */
        
        __HAL_TIM_ENABLE_IT(&g_tim_handle, TIM_IT_UPDATE);  /* ʹ��Timer�����ж� */
    }
}

/**
 * @brief       ATK_MB026 Timer�жϻص�����
 * @param       ��
 * @retval      ��
 */
void ATK_MB026_TIM_IRQHandler(void)
{
    if (__HAL_TIM_GET_FLAG(&g_tim_handle, TIM_FLAG_UPDATE) != RESET)    /* Timer�����ж� */
    {
        __HAL_TIM_CLEAR_IT(&g_tim_handle, TIM_IT_UPDATE);               /* ��������жϱ�־ */
        g_uart_rx_frame.sta.finsh = 1;                                  /* ���֡������� */
        __HAL_TIM_DISABLE(&g_tim_handle);                               /* ֹͣTimer���� */
    }
}

/**
 * @brief       ATK_MB026 UART�жϻص�����
 * @param       ��
 * @retval      ��
 */
void ATK_MB026_UART_IRQHandler(void)
{
    uint8_t tmp;
    
    if (__HAL_UART_GET_FLAG(&g_uart_handle, UART_FLAG_ORE) != RESET)        /* UART���չ��ش����ж� */
    {
        __HAL_UART_CLEAR_OREFLAG(&g_uart_handle);                           /* ������չ��ش����жϱ�־ */
        (void)g_uart_handle.Instance->SR;                                   /* �ȶ�SR�Ĵ������ٶ�DR�Ĵ��� */
        (void)g_uart_handle.Instance->DR;
    }
    
    if (__HAL_UART_GET_FLAG(&g_uart_handle, UART_FLAG_RXNE) != RESET)       /* UART�����ж� */
    {
        HAL_UART_Receive(&g_uart_handle, &tmp, 1, HAL_MAX_DELAY);           /* UART�������� */
        
        if (g_uart_rx_frame.sta.len < (ATK_MB026_UART_RX_BUF_SIZE - 1))   /* �ж�UART���ջ����Ƿ����
                                                                             * ����һλ��������'\0'
                                                                             */
        {
            __HAL_TIM_SET_COUNTER(&g_tim_handle, 0);                        /* ����Timer����ֵ */
            if (g_uart_rx_frame.sta.len == 0)                               /* �����һ֡�ĵ�һ������ */
            {
                __HAL_TIM_ENABLE(&g_tim_handle);                            /* ����Timer���� */
            }
            g_uart_rx_frame.buf[g_uart_rx_frame.sta.len] = tmp;             /* �����յ�������д�뻺�� */
            g_uart_rx_frame.sta.len++;                                      /* ���½��յ������ݳ��� */
        }
        else                                                                /* UART���ջ������ */
        {
            g_uart_rx_frame.sta.len = 0;                                    /* ����֮ǰ�յ������� */
            g_uart_rx_frame.buf[g_uart_rx_frame.sta.len] = tmp;             /* �����յ�������д�뻺�� */
            g_uart_rx_frame.sta.len++;                                      /* ���½��յ������ݳ��� */
        }
    }
}
