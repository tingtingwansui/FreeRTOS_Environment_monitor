/**
 ****************************************************************************************************
 * @file        atk_mb026.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2024-11-28
 * @brief       ATK_MB026ģ����������
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

#ifndef __ATK_MB026_H
#define __ATK_MB026_H

#include "./SYSTEM/sys/sys.h"
#include "./BSP/ATK_MB026/atk_mb026_uart.h"

/* ���Ŷ��� */
#define ATK_MB026_RST_GPIO_PORT           GPIOA
#define ATK_MB026_RST_GPIO_PIN            GPIO_PIN_4
#define ATK_MB026_RST_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0) /* PB��ʱ��ʹ�� */

/* IO���� */
#define ATK_MB026_RST(x)                  do{ x ?                                                                                     \
                                                HAL_GPIO_WritePin(ATK_MB026_RST_GPIO_PORT, ATK_MB026_RST_GPIO_PIN, GPIO_PIN_SET) :  \
                                                HAL_GPIO_WritePin(ATK_MB026_RST_GPIO_PORT, ATK_MB026_RST_GPIO_PIN, GPIO_PIN_RESET); \
                                            }while(0)

/* ����״̬ö�� */
typedef enum
{
    ATK_MB026_CONNECTED = 0x00,             /* ������ */
    ATK_MB026_DISCONNECTED,                 /* δ���� */
    ATK_MB026_DATA,                         /* �ȶ����ӿ��Դ������ݵ�״̬ */
} atk_mb026_conn_sta_t;

/* ������� */
#define ATK_MB026_EOK         0             /* û�д��� */
#define ATK_MB026_ERROR       1             /* ͨ�ô��� */
#define ATK_MB026_ETIMEOUT    2             /* ��ʱ���� */
#define ATK_MB026_EINVAL      3             /* �������� */

/* �������� */
void atk_mb026_hw_reset(void);                                                                                                                                                  /* ATK_MB026Ӳ����λ */
uint8_t atk_mb026_send_at_cmd(char *cmd, char *ack, uint32_t timeout);                                                                                                          /* ATK_MB026����ATָ�� */
uint8_t atk_mb026_send_at_cmd_task(char *cmd, char *ack, uint32_t timeout);                                                                                                     /* ATK_MB026����ATָ�����������ڲ� */
uint8_t atk_mb026_init(uint32_t baudrate);                                                                                                                                      /* ATK_MB026��ʼ�� */
uint8_t atk_mb026_restore(void);                                                                                                                                                /* ATK_MB026�ָ��������� */
uint8_t atk_mb026_at_test(void);                                                                                                                                                /* ATK_MB026 ATָ����� */
uint8_t atk_mb026_at_sysmsg(uint8_t mode);                                                                                                                                      /* ATK_MB026����ϵͳ��ʾ��Ϣ */
uint8_t atk_mb026_ble_init(uint8_t mode);                                                                                                                                       /* ATK_MB026������ʼ�� */
uint8_t atk_mb026_ble_gatts(void);                                                                                                                                              /* ATK_MB026 GATTS�������� */
uint8_t atk_mb026_ble_gattssrvstart(void);                                                                                                                                      /* ATK_MB026 GATTS����ȫ������ */
uint8_t atk_mb026_get_addr(char *buf);                                                                                                                                          /* ATK_MB026��ȡ�����豸��ַ */
uint8_t atk_mb026_set_addr(uint8_t mode);                                                                                                                                       /* ATK_MB026���������豸��ַ���� */
uint8_t atk_mb026_get_advparam(char *buf);                                                                                                                                      /* ATK_MB026��ȡ�����豸�㲥���� */
uint8_t atk_mb026_set_advparam(char *adv_int_min, char *adv_int_max, char *adv_type, char *own_addr_type, char *channel_map);                                                   /* ATK_MB026���ù㲥���� */
uint8_t atk_mb026_set_advdata(char *adv_data);                                                                                                                                  /* ATK_MB026���ù㲥���� */
uint8_t atk_mb026_set_advdataex(char *dev_name, char *uuid, char *manufacturer_data, uint16_t include_power);                                                                   /* ATK_MB026�Զ����ù㲥���� */
uint8_t atk_mb026_advstart(void);                                                                                                                                               /* ATK_MB026��ʼBluetooth LE�㲥 */
uint8_t atk_mb026_set_sppcfg(uint16_t cfg_enable, uint16_t tx_service_index, uint16_t tx_char_index, uint16_t rx_service_index, uint16_t rx_char_index, uint16_t auto_conn);    /* ATK_MB026����Bluetooth LE SPP���� */
uint8_t atk_mb026_lespp(void);                                                                                                                                                  /* ATK_MB026����Bluetooth LE SPPģʽ */
atk_mb026_conn_sta_t atk_mb026_get_conn_sta(void);                                                                                                                              /* ��ȡATK-MB026����״̬ */
uint8_t atk_mb026_set_mode(uint8_t mode);                                                                                                                                       /* ����ATK_MB026����ģʽ */
uint8_t atk_mb026_sw_reset(void);                                                                                                                                               /* ATK_MB026�����λ */
uint8_t atk_mb026_ate_config(uint8_t cfg);                                                                                                                                      /* ATK_MB026���û���ģʽ */
uint8_t atk_mb026_join_ap(char *ssid, char *pwd);                                                                                                                               /* ATK_MB026����WIFI */
uint8_t atk_mb026_get_ip(char *buf);                                                                                                                                            /* ATK_MB026��ȡIP��ַ */
uint8_t atk_mb026_connect_tcp_server(char *server_ip, char *server_port);                                                                                                       /* ATK_MB026����TCP������ */
uint8_t atk_mb026_enter_unvarnished(void);                                                                                                                                      /* ATK_MB026����͸�� */
void atk_mb026_exit_unvarnished(void);                                                                                                                                          /* ATK_MB026�˳�͸�� */
uint8_t atk_mb026_connect_atkcld(char *id, char *pwd);                                                                                                                          /* ATK_MB026����ԭ���Ʒ����� */
uint8_t atk_mb026_connect_atknewcld(char *id, char *pwd);                                                                                                                       /* ATK_MB026�����°�ԭ���Ʒ����� */
uint8_t atk_mb026_disconnect_atkcld(void);                                                                                                                                      /* ATK_MB026�Ͽ�ԭ���Ʒ��������� */

#endif
