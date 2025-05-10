/**
 ****************************************************************************************************
 * @file        atk_fire.c
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

#include "./BSP/ATK_FIRE/atk_fire.h"
#include "./BSP/ADC/adc.h"


/**
 * @brief       ���洫����ģ���ʼ������
 * @param       ��
 * @retval      ��
 */
void atk_fire_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    ATK_FIRE_DO_GPIO_CLK_ENABLE();                              /* DOʱ��ʹ�� */

    gpio_init_struct.Pin = ATK_FIRE_DO_GPIO_PIN;                /* DO���� */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                    /* ���� */
    gpio_init_struct.Pull = GPIO_NOPULL;                        /* ������ ������ */
    HAL_GPIO_Init(ATK_FIRE_DO_GPIO_PORT, &gpio_init_struct);    /* DO���ų�ʼ�� */

    adc_init();                                                 /* ��ʼ��ADC */
}

/**
 * @brief       ��ȡ���洫����ֵ
 * @param       ��
 * @retval      0~100:0,����;100,��ǿ
 */
uint8_t atk_fire_get_val(void)
{
    uint32_t temp_val = 0;
    temp_val = adc_get_result_average(ADC_ADCX_3_CHY, 10);  /* ��ȡƽ��ֵ */
    temp_val /= 40;

    if (temp_val > 100)temp_val = 100;

    return (uint8_t)(100 - temp_val);
}
