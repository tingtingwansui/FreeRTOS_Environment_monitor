/**
 ****************************************************************************************************
 * @file        atk_air.c
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

#include "./BSP/ATK_AIR/atk_air.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/ADC/adc.h"
#include "math.h"


/**
 * @brief       ��������������ģ���ʼ������
 * @param       ��
 * @retval      ��
 */
void atk_air_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    ATK_AIR_DO_GPIO_CLK_ENABLE();                                   /* DOʱ��ʹ�� */

    gpio_init_struct.Pin = ATK_AIR_DO_GPIO_PIN;                     /* DO���� */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                        /* ���� */
    gpio_init_struct.Pull = GPIO_NOPULL;                            /* �������� */
    HAL_GPIO_Init(ATK_AIR_DO_GPIO_PORT, &gpio_init_struct);         /* DO���ų�ʼ�� */

    adc_init();                                                     /* ��ʼ��ADC */
}

/**
 * @brief       ��ȡAO�˵�ѹת��ֵ
 * @param       ��
 * @retval      AO�˵�ѹת��ֵ
 */
uint32_t atk_air_adc_converted_value(void)
{
    return adc_get_result_average(ADC_ADCX_2_CHY, 10);
}

/**
 * @brief  ��ppmֵ����
 * @param  ��
 * @retval ppm
 * @note
 * ����������MQ-135_������ṩ�Ĵ�����������������������ϳ��ݺ���
 * ����Rs/R0����ppm��������Ϻ���ʱ��x��ΪRs/R0��y��Ϊppm���Ƶ���ʽ��Ϊ��y = ax^b
 * ������ϵ�����û��ÿ����ľ�����ֵ������ֻ�ܴ��¹��㣬���������ҿ��Զ�α궨�������
 */
float atk_air_get_ppm(void)
{
    uint32_t adc_converted_value;       /* AO�˵�ѹת��ֵ */
    float vrl;                          /* AO�˵ĵ�ѹ�������������ĸ��ص���RL�ϵĵ�ѹ�� */
    float rs;                           /* ��ǰ�������ĵ��� */
    float ppm;                          /* ��ȼ����ƽ��Ũ�� */
    float rs0;                          /* ����������ȣ��ڲ��Ի�������������ֵ�����ڽྻ������������������ֵ�� */
    
    adc_converted_value = atk_air_adc_converted_value();
    //printf("AO�˵�ѹת��ֵΪ��%d\r\n", adc_converted_value);
    
    vrl = (float)adc_converted_value / 4096.f * VREF;  /* ����AO�˵ĵ�ѹ */
    //printf("AO�˵�ѹ��vrl��Ϊ��%.3fV\r\n", vrl);
    
    rs = (float)(VC - vrl) * RL / vrl;                 /* ���㴫��������ֵ */
    //printf("��ǰMQ135����������ֵΪ��%.1fk��\r\n", rs);
    
    rs0 = rs / R0;  /* Rs / R0 */
    //printf("Rs/R0 = %0.3f\r\n", rs0);

    /* y = ax^b�� ����xΪRs/R0��a��b��ȡֵ���������ֲ�ͼ��������ϳ��ݺ��� */
    ppm =  A * pow(rs0, B);

    return ppm;
}
