#include "./BSP/ATK_MB026/atk_mb026.h"
#include "./SYSTEM/delay/delay.h"
#include <string.h>
#include <stdio.h>
//FreeRTOS����
#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief       ATK_MB026Ӳ����ʼ��
 * @param       ��
 * @retval      ��
 */
static void atk_mb026_hw_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    ATK_MB026_RST_GPIO_CLK_ENABLE();
    
    gpio_init_struct.Pin = ATK_MB026_RST_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ATK_MB026_RST_GPIO_PORT, &gpio_init_struct);
}

/**
 * @brief       ATK_MB026Ӳ����λ
 * @param       ��
 * @retval      ��
 */
void atk_mb026_hw_reset(void)
{
    ATK_MB026_RST(0);
    delay_ms(100);
    ATK_MB026_RST(1);
    delay_ms(500);
}

/**
 * @brief       ATK_MB026����ATָ��
 * @param       cmd    : �����͵�ATָ��
 *              ack    : �ȴ�����Ӧ
 *              timeout: �ȴ���ʱʱ��
 * @retval      ATK_MB026_EOK     : ����ִ�гɹ�
 *              ATK_MB026_ETIMEOUT: �ȴ�����Ӧ��ʱ������ִ��ʧ��
 */
uint8_t atk_mb026_send_at_cmd(char *cmd, char *ack, uint32_t timeout)
{
    uint8_t *ret = NULL;
    
    atk_mb026_uart_rx_restart();
    atk_mb026_uart_printf("%s\r\n", cmd);
    
    if ((ack == NULL) || (timeout == 0))
    {
        return ATK_MB026_EOK;
    }
    else
    {
        while (timeout > 0)
        {
            ret = atk_mb026_uart_rx_get_frame();
            if (ret != NULL)
            {
                if (strstr((const char *)ret, ack) != NULL)
                {
                    return ATK_MB026_EOK;
                }
                else
                {
                    atk_mb026_uart_rx_restart();
                }
            }
            timeout--;
            delay_ms(1);
        }
        
        return ATK_MB026_ETIMEOUT;
    }
}

//ר�����������ATָ���
uint8_t atk_mb026_send_at_cmd_task(char *cmd, char *ack, uint32_t timeout)
{
    uint8_t *ret = NULL;
    
    atk_mb026_uart_rx_restart();
    atk_mb026_uart_printf("%s\r\n", cmd);
    
    if ((ack == NULL) || (timeout == 0))
    {
        return ATK_MB026_EOK;
    }
    else
    {
        while (timeout > 0)
        {
            ret = atk_mb026_uart_rx_get_frame();
            if (ret != NULL)
            {
                if (strstr((const char *)ret, ack) != NULL)
                {
                    return ATK_MB026_EOK;
                }
                else
                {
                    atk_mb026_uart_rx_restart();
                }
            }
            timeout--;
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        
        return ATK_MB026_ETIMEOUT;
    }
}

/**
 * @brief       ATK_MB026��ʼ��
 * @param       baudrate: ATK_MB026 UARTͨѶ������
 * @retval      ATK_MB026_EOK  : ATK_MB026��ʼ���ɹ�������ִ�гɹ�
 *              ATK_MB026_ERROR: ATK_MB026��ʼ��ʧ�ܣ�����ִ��ʧ��
 */
uint8_t atk_mb026_init(uint32_t baudrate)
{
    delay_ms(800);
    atk_mb026_hw_init();                          /* ATK_MB026Ӳ����ʼ�� */
    atk_mb026_hw_reset();                         /* ATK_MB026Ӳ����λ */
    atk_mb026_uart_init(baudrate);                /* ATK_MB026 UART��ʼ�� */
    
    if (atk_mb026_at_test() != ATK_MB026_EOK)     /* ATK_MB026 ATָ����� */
    {
        return ATK_MB026_ERROR;
    }
    
    return ATK_MB026_EOK;
}

/**
 * @brief       ATK_MB026�ָ���������
 * @param       ��
 * @retval      ATK_MB026_EOK  : �ָ��������óɹ�
 *              ATK_MB026_ERROR: �ָ���������ʧ��
 */
uint8_t atk_mb026_restore(void)
{
    uint8_t ret;

    ret = atk_mb026_send_at_cmd("AT+RESTORE", "ready", 3000);
    
    if (ret == ATK_MB026_EOK)
    {
        return ATK_MB026_EOK;
    }
    else
    {
        return ATK_MB026_ERROR;
    }
}

/**
 * @brief       ATK_MB026 ATָ�����
 * @param       ��
 * @retval      ATK_MB026_EOK  : ATָ����Գɹ�
 *              ATK_MB026_ERROR: ATָ�����ʧ��
 */
uint8_t atk_mb026_at_test(void)
{
    uint8_t ret;
    uint8_t i;
    
    for (i=0; i<10; i++)
    {
        ret = atk_mb026_send_at_cmd("AT", "OK", 500);
        if (ret == ATK_MB026_EOK)
        {
            return ATK_MB026_EOK;
        }
    }
    
    return ATK_MB026_ERROR;
}

/**
 * @brief       ATK_MB026����ϵͳ��ʾ��Ϣ
 * @param       mode: 1������ӡ��Ϣ
 *                    2������ʱ��ӡ��ϸ����ʾ��Ϣ
 *                    3��͸��ģʽ�£�Wi-Fi��socket��Bluetooth LE �� Bluetooth ״̬�ı�ʱ���ӡ��ʾ��
 *              ATK_MB026_ERROR : ATK_MB026������ʼ��ʧ��
 *              ATK_MB026_EINVAL: mode��������ATK_MB026������ʼ��ʧ��
 */
uint8_t atk_mb026_at_sysmsg(uint8_t mode)
{
    uint8_t ret;
    
    switch (mode)
    {
        case 1:
        {
            ret = atk_mb026_send_at_cmd("AT+SYSMSG=0", "OK", 500);    /* ����ӡ��Ϣ */
            break;
        }
        case 2:
        {
            ret = atk_mb026_send_at_cmd("AT+SYSMSG=2", "OK", 500);    /* ����ʱ��ӡ��ϸ����ʾ��Ϣ */
            break;
        }
        case 3:
        {
            ret = atk_mb026_send_at_cmd("AT+SYSMSG=4", "OK", 500);    /* ͸��ģʽ�£�Wi-Fi��socket��Bluetooth LE �� Bluetooth ״̬�ı�ʱ���ӡ��ʾ�� */
            break;
        }
        default:
        {
            return ATK_MB026_EINVAL;
        }
    }
    
    if (ret == ATK_MB026_EOK)
    {
        return ATK_MB026_EOK;
    }
    else
    {
        return ATK_MB026_ERROR;
    }
}

/**
 * @brief       ATK_MB026������ʼ��
 * @param       mode: 1��ע��Bluetooth LE
 *                    2��client��ɫ
 *                    3��server��ɫ
 * @retval      ATK_MB026_EOK   : ATK_MB026������ʼ���ɹ�
 *              ATK_MB026_ERROR : ATK_MB026������ʼ��ʧ��
 *              ATK_MB026_EINVAL: mode��������ATK_MB026������ʼ��ʧ��
 */
uint8_t atk_mb026_ble_init(uint8_t mode)
{
    uint8_t ret;
    
    switch (mode)
    {
        case 1:
        {
            ret = atk_mb026_send_at_cmd("AT+BLEINIT=0", "OK", 500);    /* ע��Bluetooth LE */
            break;
        }
        case 2:
        {
            ret = atk_mb026_send_at_cmd("AT+BLEINIT=1", "OK", 500);    /* client��ɫ */
            break;
        }
        case 3:
        {
            ret = atk_mb026_send_at_cmd("AT+BLEINIT=2", "OK", 500);    /* server��ɫ */
            break;
        }
        default:
        {
            return ATK_MB026_EINVAL;
        }
    }
    
    if (ret == ATK_MB026_EOK)
    {
        return ATK_MB026_EOK;
    }
    else
    {
        return ATK_MB026_ERROR;
    }
}

/**
 * @brief       GATTS��������
 * @param       ��
 * @retval      ATK_MB026_EOK  : ��������ɹ�
 *              ATK_MB026_ERROR: ��������ʧ��
 */
uint8_t atk_mb026_ble_gatts(void)
{
    uint8_t ret;

    ret = atk_mb026_send_at_cmd("AT+BLEGATTSSRVCRE", "OK", 1000);
    
    if (ret == ATK_MB026_EOK)
    {
        return ATK_MB026_EOK;
    }
    else
    {
        return ATK_MB026_ERROR;
    }
}

/**
 * @brief       GATTS����ȫ������
 * @param       ��
 * @retval      ATK_MB026_EOK  : ��������ɹ�
 *              ATK_MB026_ERROR: ��������ʧ��
 */
uint8_t atk_mb026_ble_gattssrvstart(void)
{
    uint8_t ret;
    
    ret = atk_mb026_send_at_cmd("AT+BLEGATTSSRVSTART", "OK", 1000);

    if (ret == ATK_MB026_EOK)
    {
        return ATK_MB026_EOK;
    }
    else
    {
        return ATK_MB026_ERROR;
    }
}

/**
 * @brief       ATK_MB026��ȡ�����豸��ַ
 * @param       buf: IP��ַ����Ҫ16�ֽ��ڴ�ռ�
 * @retval      ATK_MB026_EOK  : ��ȡ��ַ�ɹ�
 *              ATK_MB026_ERROR: ��ȡ��ַʧ��
 */
uint8_t atk_mb026_get_addr(char *buf)
{
    uint8_t ret;
    char *p_start;
    char *p_end;
    
    ret = atk_mb026_send_at_cmd("AT+BLEADDR?", "OK", 500);
    if (ret != ATK_MB026_EOK)
    {
        return ATK_MB026_ERROR;
    }
    
    p_start = strstr((const char *)atk_mb026_uart_rx_get_frame(), "\"");
    p_end = strstr(p_start + 1, "\"");
    *p_end = '\0';
    sprintf(buf, "%s", p_start + 1);
    
    return ATK_MB026_EOK;
}

/**
 * @brief       ATK_MB026���������豸��ַ����
 * @param       mode: 0��������ַ(Public Address)
 *                    1�������ַ(Random Address)
 * @retval      ATK_MB026_EOK   : ATK_MB026�����豸��ַ�������óɹ�
 *              ATK_MB026_ERROR : ATK_MB026�����豸��ַ��������ʧ��
 *              ATK_MB026_EINVAL: mode��������ATK_MB026�����豸��ַ��������ʧ��
 */
uint8_t atk_mb026_set_addr(uint8_t mode)
{
    uint8_t ret;
    
    switch (mode)
    {
        case 1:
        {
            ret = atk_mb026_send_at_cmd("AT+BLEADDR=0", "OK", 500);    /* ������ַ */
            break;
        }
        case 2:
        {
            ret = atk_mb026_send_at_cmd("AT+BLEADDR=1", "OK", 500);    /* �����ַ */
            break;
        }
        default:
        {
            return ATK_MB026_EINVAL;
        }
    }
    
    if (ret == ATK_MB026_EOK)
    {
        return ATK_MB026_EOK;
    }
    else
    {
        return ATK_MB026_ERROR;
    }
}

/**
 * @brief       ATK_MB026��ȡ�����豸�㲥����
 * @param       buf: �����豸��������Ҫ60�ֽ��ڴ�ռ�
 * @retval      ATK_MB026_EOK  : ��ȡ��ַ�ɹ�
 *              ATK_MB026_ERROR: ��ȡ��ַʧ��
 */
uint8_t atk_mb026_get_advparam(char *buf)
{
    uint8_t ret;
    char *p_start;
    char *p_end;
    
    ret = atk_mb026_send_at_cmd("AT+BLEADVPARAM?", "OK", 500);
    if (ret != ATK_MB026_EOK)
    {
        return ATK_MB026_ERROR;
    }
    
    p_start = strstr((const char *)atk_mb026_uart_rx_get_frame(), "\"");
    p_end = strstr(p_start + 1, "\"");
    *p_end = '\0';
    sprintf(buf, "%s", p_start + 1);
    
    return ATK_MB026_EOK;
}

/**
 * @brief       ATK_MB026���ù㲥����
 * @param       adv_int_min: ��С�㲥���
 *              adv_int_max : ���㲥���
 *              adv_type : �㲥����[0-7]
 *              own_addr_type : Blutooth LE ��ַ����
 *              channel_map : �㲥�ŵ�
 *              ��δʹ�ø��������ù㲥��������ʹ��Ĭ�Ϲ㲥����
 * @retval      ATK_MB026_EOK  : �㲥�������óɹ�
 *              ATK_MB026_ERROR: �㲥��������ʧ��
 */
uint8_t atk_mb026_set_advparam(char *adv_int_min, char *adv_int_max, char *adv_type, char *own_addr_type, char *channel_map)
{
    uint8_t ret;
    char cmd[64];
    
    sprintf(cmd, "AT+BLEADVPARAM=\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"", adv_int_min, adv_int_max, adv_type, own_addr_type, channel_map);
    ret = atk_mb026_send_at_cmd(cmd, "OK", 10000);
    
    if (ret == ATK_MB026_EOK)
    {
        return ATK_MB026_EOK;
    }
    else
    {
        return ATK_MB026_ERROR;
    }
}

/**
 * @brief       ATK_MB026���ù㲥����
 * @param       adv_data: �㲥���ݣ�ΪHEX�ַ�����
 *              ����������һ��豸��ʹ��Ĭ�ϵĹ㲥����
 * @retval      ATK_MB026_EOK  : �㲥�������óɹ�
 *              ATK_MB026_ERROR: �㲥��������ʧ��
 */
uint8_t atk_mb026_set_advdata(char *adv_data)
{
    uint8_t ret;
    char cmd[64];
    
    sprintf(cmd, "AT+BLEADVDATA=\"%s\"", adv_data);
    ret = atk_mb026_send_at_cmd(cmd, "OK", 10000);
    
    if (ret == ATK_MB026_EOK)
    {
        return ATK_MB026_EOK;
    }
    else
    {
        return ATK_MB026_ERROR;
    }
}

/**
 * @brief       ATK_MB026�Զ����ù㲥����
 * @param       dev_name: �ַ�����������ʾ�豸����
 *              uuid : �ַ�����������ʶ��
 *              manufacturer_data : ʮ�������ַ�������������������
 *              include_power : ���㲥��������� TX ���ʣ�������Ӧ����Ϊ 1������Ϊ 0
 *              ��δʹ�ø��������ù㲥��������ʹ��Ĭ�Ϲ㲥����
 * @retval      ATK_MB026_EOK  : �Զ����ù㲥�������óɹ�
 *              ATK_MB026_ERROR: �Զ����ù㲥��������ʧ��
 */
uint8_t atk_mb026_set_advdataex(char *dev_name, char *uuid, char *manufacturer_data, uint16_t include_power)
{
    uint8_t ret;
    char cmd[80];
    
    sprintf(cmd, "AT+BLEADVDATAEX=\"%s\",\"%s\",\"%s\",%d", dev_name, uuid, manufacturer_data, include_power);
    ret = atk_mb026_send_at_cmd(cmd, "OK", 1000);
    delay_ms(500);
    if (ret == ATK_MB026_EOK)
    {
        return ATK_MB026_EOK;
    }
    else
    {
        return ATK_MB026_ERROR;
    }
}

/**
 * @brief       ATK_MB026��ʼBluetooth LE�㲥
 * @param       ��
 * @retval      ATK_MB026_EOK  : Bluetooth LE�㲥�ɹ�
 *              ATK_MB026_ERROR: Bluetooth LE�㲥ʧ��
 */
uint8_t atk_mb026_advstart(void)
{
    uint8_t ret;
    
    ret = atk_mb026_send_at_cmd("AT+BLEADVSTART", "OK", 500);
    
    if (ret == ATK_MB026_EOK)
    {
        return ATK_MB026_EOK;
    }
    else
    {
        return ATK_MB026_ERROR;
    }
}

/**
 * @brief       ATK_MB026����Bluetooth LE SPP����
 * @param       cfg_enable:        0���������� SPP �������������������д
 *                                 1�����������Ҫ��д
 *              tx_service_index : tx �������
 *              tx_char_index :    tx �����������
 *              rx_service_index : rx �������
 *              rx_char_index :    rx �����������
 *              auto_conn :        �Զ�������־λ��Ĭ������£��Զ��������ܱ�ʹ��
 *                                 0: ��ֹ Bluetooth LE ͸���Զ��������ܡ�
 *                                 1: ʹ�� Bluetooth LE ͸���Զ���������
 * @retval      ATK_MB026_EOK  : Bluetooth LE SPP�������óɹ�
 *              ATK_MB026_ERROR: Bluetooth LE SPP��������ʧ��
 */
uint8_t atk_mb026_set_sppcfg(uint16_t cfg_enable, uint16_t tx_service_index, uint16_t tx_char_index, uint16_t rx_service_index, uint16_t rx_char_index, uint16_t auto_conn)
{
    uint8_t ret;
    char cmd[30];
    
    sprintf(cmd, "AT+BLESPPCFG=%d,%d,%d,%d,%d,%d", cfg_enable, tx_service_index, tx_char_index, rx_service_index, rx_char_index, auto_conn);
    ret = atk_mb026_send_at_cmd(cmd, "OK", 1000);

    if (ret != ATK_MB026_EOK)
    {
        return ATK_MB026_ERROR;
    }
    
    return ATK_MB026_EOK;
}

/**
 * @brief       ATK_MB026����Bluetooth LE SPPģʽ
 * @param       ��
 * @retval      ATK_MB026_EOK  : ����Bluetooth LE SPPģʽ�ɹ�
 *              ATK_MB026_ERROR: ����Bluetooth LE SPPģʽʧ��
 */
uint8_t atk_mb026_lespp(void)
{
    uint8_t ret;
    
    ret = atk_mb026_send_at_cmd("AT+BLESPP", "OK", 1000);
    if (ret == ATK_MB026_EOK)
    {
        return ATK_MB026_EOK;
    }
    else
    {
        return ATK_MB026_ERROR;
    }
}

/**
 * @brief       ��ȡATK-MB026����״̬
 * @param       ��
 * @retval      ATK_MB026_CONNECTED   : ������
 *              ATK_MB026_DISCONNECTED: δ����
 */
atk_mb026_conn_sta_t atk_mb026_get_conn_sta(void)
{
    uint8_t *recv_len;
    
    recv_len = atk_mb026_uart_rx_get_frame();
    
    if (strstr((char *)recv_len, "+BLECONN") != NULL)
    {
        printf("\r\n Connected!\r\n");
        
        return ATK_MB026_CONNECTED;
    }
    
    if (strstr((char *)recv_len, "+BLEDISCONN") != NULL)
    {
        printf("\r\n Not connected!\r\n");
        
        return ATK_MB026_DISCONNECTED;
    }
    
    return ATK_MB026_DATA;
}

/**
 * @brief       ����ATK_MB026����ģʽ
 * @param       mode: 1��Stationģʽ
 *                    2��APģʽ
 *                    3��AP+Stationģʽ
 * @retval      ATK_MB026_EOK   : ����ģʽ���óɹ�
 *              ATK_MB026_ERROR : ����ģʽ����ʧ��
 *              ATK_MB026_EINVAL: mode�������󣬹���ģʽ����ʧ��
 */
uint8_t atk_mb026_set_mode(uint8_t mode)
{
    uint8_t ret;
    
    switch (mode)
    {
        case 1:
        {
            ret = atk_mb026_send_at_cmd("AT+CWMODE=1", "OK", 500);    /* Stationģʽ */
            break;
        }
        case 2:
        {
            ret = atk_mb026_send_at_cmd("AT+CWMODE=2", "OK", 500);    /* APģʽ */
            break;
        }
        case 3:
        {
            ret = atk_mb026_send_at_cmd("AT+CWMODE=3", "OK", 500);    /* AP+Stationģʽ */
            break;
        }
        default:
        {
            return ATK_MB026_EINVAL;
        }
    }
    
    if (ret == ATK_MB026_EOK)
    {
        return ATK_MB026_EOK;
    }
    else
    {
        return ATK_MB026_ERROR;
    }
}

/**
 * @brief       ATK_MB026�����λ
 * @param       ��
 * @retval      ATK_MB026_EOK  : �����λ�ɹ�
 *              ATK_MB026_ERROR: �����λʧ��
 */
uint8_t atk_mb026_sw_reset(void)
{
    uint8_t ret;
    
    ret = atk_mb026_send_at_cmd("AT+RST", "OK", 500);
    if (ret == ATK_MB026_EOK)
    {
        delay_ms(1000);
        return ATK_MB026_EOK;
    }
    else
    {
        return ATK_MB026_ERROR;
    }
}

/**
 * @brief       ATK_MB026���û���ģʽ
 * @param       cfg: 0���رջ���
 *                   1���򿪻���
 * @retval      ATK_MB026_EOK  : ���û���ģʽ�ɹ�
 *              ATK_MB026_ERROR: ���û���ģʽʧ��
 */
uint8_t atk_mb026_ate_config(uint8_t cfg)
{
    uint8_t ret;
    
    switch (cfg)
    {
        case 0:
        {
            ret = atk_mb026_send_at_cmd("ATE0", "OK", 500);   /* �رջ��� */
            break;
        }
        case 1:
        {
            ret = atk_mb026_send_at_cmd("ATE1", "OK", 500);   /* �򿪻��� */
            break;
        }
        default:
        {
            return ATK_MB026_EINVAL;
        }
    }
    
    if (ret == ATK_MB026_EOK)
    {
        return ATK_MB026_EOK;
    }
    else
    {
        return ATK_MB026_ERROR;
    }
}

/**
 * @brief       ATK_MB026����WIFI
 * @param       ssid: WIFI����
 *              pwd : WIFI����
 * @retval      ATK_MB026_EOK  : WIFI���ӳɹ�
 *              ATK_MB026_ERROR: WIFI����ʧ��
 */
uint8_t atk_mb026_join_ap(char *ssid, char *pwd)
{
    uint8_t ret;
    char cmd[64];
    
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, pwd);
    ret = atk_mb026_send_at_cmd(cmd, "WIFI GOT IP", 10000);
    
    if (ret == ATK_MB026_EOK)
    {
        return ATK_MB026_EOK;
    }
    else
    {
        return ATK_MB026_ERROR;
    }
}

/**
 * @brief       ATK_MB026��ȡIP��ַ
 * @param       buf: IP��ַ����Ҫ16�ֽ��ڴ�ռ�
 * @retval      ATK_MB026_EOK  : ��ȡIP��ַ�ɹ�
 *              ATK_MB026_ERROR: ��ȡIP��ַʧ��
 */
uint8_t atk_mb026_get_ip(char *buf)
{
    uint8_t ret;
    char *p_start;
    char *p_end;
    
    ret = atk_mb026_send_at_cmd("AT+CIFSR", "OK", 500);
    if (ret != ATK_MB026_EOK)
    {
        return ATK_MB026_ERROR;
    }
    
    p_start = strstr((const char *)atk_mb026_uart_rx_get_frame(), "\"");
    p_end = strstr(p_start + 1, "\"");
    *p_end = '\0';
    sprintf(buf, "%s", p_start + 1);
    
    return ATK_MB026_EOK;
}

/**
 * @brief       ATK_MB026����TCP������
 * @param       server_ip  : TCP������IP��ַ
 *              server_port: TCP�������˿ں�
 * @retval      ATK_MB026_EOK  : ����TCP�������ɹ�
 *              ATK_MB026_ERROR: ����TCP������ʧ��
 */
uint8_t atk_mb026_connect_tcp_server(char *server_ip, char *server_port)
{
    uint8_t ret;
    char cmd[64];
    
    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%s", server_ip, server_port);
    ret = atk_mb026_send_at_cmd(cmd, "CONNECT", 5000);
    if (ret == ATK_MB026_EOK)
    {
        return ATK_MB026_EOK;
    }
    else
    {
        return ATK_MB026_ERROR;
    }
}

/**
 * @brief       ATK_MB026����͸��
 * @param       ��
 * @retval      ATK_MB026_EOK  : ����͸���ɹ�
 *              ATK_MB026_ERROR: ����͸��ʧ��
 */
uint8_t atk_mb026_enter_unvarnished(void)
{
    uint8_t ret;
    
    ret  = atk_mb026_send_at_cmd("AT+CIPMODE=1", "OK", 500);
    ret += atk_mb026_send_at_cmd("AT+CIPSEND", ">", 500);
    if (ret == ATK_MB026_EOK)
    {
        return ATK_MB026_EOK;
    }
    else
    {
        return ATK_MB026_ERROR;
    }
}

/**
 * @brief       ATK_MB026�˳�͸��
 * @param       ��
 * @retval      ��
 */
void atk_mb026_exit_unvarnished(void)
{
    atk_mb026_uart_printf("+++");
}

/**
 * @brief       ATK_MB026����ԭ���Ʒ�����
 * @param       id : ԭ�����豸���
 *              pwd: ԭ�����豸����
 * @retval      ATK_MB026_EOK  : ����ԭ���Ʒ������ɹ�
 *              ATK_MB026_ERROR: ����ԭ���Ʒ�����ʧ��
 */
uint8_t atk_mb026_connect_atkcld(char *id, char *pwd)
{
    uint8_t ret;
    char cmd[64];
    
    sprintf(cmd, "AT+ATKCLDSTA=\"%s\",\"%s\"", id, pwd);
    ret = atk_mb026_send_at_cmd(cmd, "CLOUD CONNECTED", 10000);
    if (ret == ATK_MB026_EOK)
    {
        return ATK_MB026_EOK;
    }
    else
    {
        return ATK_MB026_ERROR;
    }
}

/**
 * @brief       ATK_MB026�����°�ԭ���Ʒ�����
 * @param       id : �°�ԭ�����豸���
 *              pwd: �°�ԭ�����豸����
 * @retval      ATK_MB026_EOK  : ����ԭ���Ʒ������ɹ�
 *              ATK_MB026_ERROR: ����ԭ���Ʒ�����ʧ��
 */
uint8_t atk_mb026_connect_atknewcld(char *id, char *pwd)
{
    uint8_t ret;
    char cmd[64];
    
    sprintf(cmd, "AT+ATKNEWCLDSTA=\"%s\",\"%s\"", id, pwd);
    ret = atk_mb026_send_at_cmd(cmd, "CLOUD CONNECTED", 10000);
    if (ret == ATK_MB026_EOK)
    {
        return ATK_MB026_EOK;
    }
    else
    {
        return ATK_MB026_ERROR;
    }
}

/**
 * @brief       ATK_MB026�Ͽ�ԭ���Ʒ���������
 * @param       ��
 * @retval      ATK_MB026_EOK  : �Ͽ�ԭ���Ʒ��������ӳɹ�
 *              ATK_MB026_ERROR: �Ͽ�ԭ���Ʒ���������ʧ��
 */
uint8_t atk_mb026_disconnect_atkcld(void)
{
    uint8_t ret;
    
    ret = atk_mb026_send_at_cmd("AT+ATKCLDCLS", "CLOUD DISCONNECT", 500);
    if (ret == ATK_MB026_EOK)
    {
        return ATK_MB026_EOK;
    }
    else
    {
        return ATK_MB026_ERROR;
    }
}
