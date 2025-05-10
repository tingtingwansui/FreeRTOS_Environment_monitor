#ifndef __ADC_H
#define __ADC_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* ADC及引脚 定义 (光敏电阻)*/

#define ADC_ADCX_1_CHY_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)         /* PA口时钟使能 */

#define ADC_ADC1                            ADC1 
#define ADC_ADCX_1_CHY_CLK_ENABLE()           do{ __HAL_RCC_ADC1_CLK_ENABLE(); }while(0)          /* ADC1 时钟使能 */

/******************************************************************************************/
/* ADC及引脚 定义 (空气质量传感器)*/

#define ADC_ADCX_2_CHY_GPIO_PORT              GPIOB
#define ADC_ADCX_2_CHY_GPIO_PIN               GPIO_PIN_1
#define ADC_ADCX_2_CHY_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)         /* PA口时钟使能 */

//在ADC1
//#define ADC_ADC1                            ADC1 
#define ADC_ADCX_2_CHY                        ADC_CHANNEL_9                                       /* 通道Y,  0 <= Y <= 17 */ 
#define ADC_ADCX_2_CHY_CLK_ENABLE()           do{ __HAL_RCC_ADC1_CLK_ENABLE(); }while(0)          /* ADC1 时钟使能 */

/******************************************************************************************/
/* ADC及引脚 定义 (火焰传感器)*/

#define ADC_ADCX_3_CHY_GPIO_PORT              GPIOA
#define ADC_ADCX_3_CHY_GPIO_PIN               GPIO_PIN_5
#define ADC_ADCX_3_CHY_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)         /* PA口时钟使能 */

//在ADC1
//#define ADC_ADC1                            ADC1 
#define ADC_ADCX_3_CHY                        ADC_CHANNEL_5                                       /* 通道Y,  0 <= Y <= 17 */ 
#define ADC_ADCX_3_CHY_CLK_ENABLE()           do{ __HAL_RCC_ADC1_CLK_ENABLE(); }while(0)          /* ADC1 时钟使能 */

/******************************************************************************************/
/* 外部接口函数*/
void adc_init(void);                                                                            /* ADC初始化 */
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch,uint32_t rank, uint32_t stime); /* ADC通道设置 */
uint32_t adc_get_result(uint32_t ch);                                                           /* 获得某个通道值  */
uint32_t adc_get_result_average(uint32_t ch, uint8_t times);                                    /* 得到某个通道给定次数采样的平均值 */

#endif 















