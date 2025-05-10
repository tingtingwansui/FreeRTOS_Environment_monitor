#include "./BSP/ATK_MB026/atk_mb026.h"
#include "./SYSTEM/delay/delay.h"
#include <string.h>
#include <stdio.h>
//FreeRTOS配置
#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief       ATK_MB026硬件初始化
 * @param       无
 * @retval      无
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
 * @brief       ATK_MB026硬件复位
 * @param       无
 * @retval      无
 */
void atk_mb026_hw_reset(void)
{
    ATK_MB026_RST(0);
    delay_ms(100);
    ATK_MB026_RST(1);
    delay_ms(500);
}

/**
 * @brief       ATK_MB026发送AT指令
 * @param       cmd    : 待发送的AT指令
 *              ack    : 等待的响应
 *              timeout: 等待超时时间
 * @retval      ATK_MB026_EOK     : 函数执行成功
 *              ATK_MB026_ETIMEOUT: 等待期望应答超时，函数执行失败
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

//专门用于任务的AT指令发送
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
 * @brief       ATK_MB026初始化
 * @param       baudrate: ATK_MB026 UART通讯波特率
 * @retval      ATK_MB026_EOK  : ATK_MB026初始化成功，函数执行成功
 *              ATK_MB026_ERROR: ATK_MB026初始化失败，函数执行失败
 */
uint8_t atk_mb026_init(uint32_t baudrate)
{
    delay_ms(800);
    atk_mb026_hw_init();                          /* ATK_MB026硬件初始化 */
    atk_mb026_hw_reset();                         /* ATK_MB026硬件复位 */
    atk_mb026_uart_init(baudrate);                /* ATK_MB026 UART初始化 */
    
    if (atk_mb026_at_test() != ATK_MB026_EOK)     /* ATK_MB026 AT指令测试 */
    {
        return ATK_MB026_ERROR;
    }
    
    return ATK_MB026_EOK;
}

/**
 * @brief       ATK_MB026恢复出厂设置
 * @param       无
 * @retval      ATK_MB026_EOK  : 恢复出场设置成功
 *              ATK_MB026_ERROR: 恢复出场设置失败
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
 * @brief       ATK_MB026 AT指令测试
 * @param       无
 * @retval      ATK_MB026_EOK  : AT指令测试成功
 *              ATK_MB026_ERROR: AT指令测试失败
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
 * @brief       ATK_MB026设置系统提示信息
 * @param       mode: 1，不打印信息
 *                    2，连接时打印详细版提示信息
 *                    3，透传模式下，Wi-Fi、socket、Bluetooth LE 或 Bluetooth 状态改变时会打印提示信
 *              ATK_MB026_ERROR : ATK_MB026蓝牙初始化失败
 *              ATK_MB026_EINVAL: mode参数错误，ATK_MB026蓝牙初始化失败
 */
uint8_t atk_mb026_at_sysmsg(uint8_t mode)
{
    uint8_t ret;
    
    switch (mode)
    {
        case 1:
        {
            ret = atk_mb026_send_at_cmd("AT+SYSMSG=0", "OK", 500);    /* 不打印信息 */
            break;
        }
        case 2:
        {
            ret = atk_mb026_send_at_cmd("AT+SYSMSG=2", "OK", 500);    /* 连接时打印详细版提示信息 */
            break;
        }
        case 3:
        {
            ret = atk_mb026_send_at_cmd("AT+SYSMSG=4", "OK", 500);    /* 透传模式下，Wi-Fi、socket、Bluetooth LE 或 Bluetooth 状态改变时会打印提示信 */
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
 * @brief       ATK_MB026蓝牙初始化
 * @param       mode: 1，注销Bluetooth LE
 *                    2，client角色
 *                    3，server角色
 * @retval      ATK_MB026_EOK   : ATK_MB026蓝牙初始化成功
 *              ATK_MB026_ERROR : ATK_MB026蓝牙初始化失败
 *              ATK_MB026_EINVAL: mode参数错误，ATK_MB026蓝牙初始化失败
 */
uint8_t atk_mb026_ble_init(uint8_t mode)
{
    uint8_t ret;
    
    switch (mode)
    {
        case 1:
        {
            ret = atk_mb026_send_at_cmd("AT+BLEINIT=0", "OK", 500);    /* 注销Bluetooth LE */
            break;
        }
        case 2:
        {
            ret = atk_mb026_send_at_cmd("AT+BLEINIT=1", "OK", 500);    /* client角色 */
            break;
        }
        case 3:
        {
            ret = atk_mb026_send_at_cmd("AT+BLEINIT=2", "OK", 500);    /* server角色 */
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
 * @brief       GATTS创建服务
 * @param       无
 * @retval      ATK_MB026_EOK  : 创建服务成功
 *              ATK_MB026_ERROR: 创建服务失败
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
 * @brief       GATTS开启全部服务
 * @param       无
 * @retval      ATK_MB026_EOK  : 开启服务成功
 *              ATK_MB026_ERROR: 开启服务失败
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
 * @brief       ATK_MB026获取蓝牙设备地址
 * @param       buf: IP地址，需要16字节内存空间
 * @retval      ATK_MB026_EOK  : 获取地址成功
 *              ATK_MB026_ERROR: 获取地址失败
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
 * @brief       ATK_MB026设置蓝牙设备地址类型
 * @param       mode: 0，公共地址(Public Address)
 *                    1，随机地址(Random Address)
 * @retval      ATK_MB026_EOK   : ATK_MB026蓝牙设备地址类型设置成功
 *              ATK_MB026_ERROR : ATK_MB026蓝牙设备地址类型设置失败
 *              ATK_MB026_EINVAL: mode参数错误，ATK_MB026蓝牙设备地址类型设置失败
 */
uint8_t atk_mb026_set_addr(uint8_t mode)
{
    uint8_t ret;
    
    switch (mode)
    {
        case 1:
        {
            ret = atk_mb026_send_at_cmd("AT+BLEADDR=0", "OK", 500);    /* 公共地址 */
            break;
        }
        case 2:
        {
            ret = atk_mb026_send_at_cmd("AT+BLEADDR=1", "OK", 500);    /* 随机地址 */
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
 * @brief       ATK_MB026获取蓝牙设备广播参数
 * @param       buf: 蓝牙设备参数，需要60字节内存空间
 * @retval      ATK_MB026_EOK  : 获取地址成功
 *              ATK_MB026_ERROR: 获取地址失败
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
 * @brief       ATK_MB026设置广播参数
 * @param       adv_int_min: 最小广播间隔
 *              adv_int_max : 最大广播间隔
 *              adv_type : 广播类型[0-7]
 *              own_addr_type : Blutooth LE 地址类型
 *              channel_map : 广播信道
 *              若未使用该命令设置广播参数，则使用默认广播参数
 * @retval      ATK_MB026_EOK  : 广播参数设置成功
 *              ATK_MB026_ERROR: 广播参数设置失败
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
 * @brief       ATK_MB026设置广播数据
 * @param       adv_data: 广播数据，为HEX字符串，
 *              若不设置这一项，设备将使用默认的广播名称
 * @retval      ATK_MB026_EOK  : 广播数据设置成功
 *              ATK_MB026_ERROR: 广播数据设置失败
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
 * @brief       ATK_MB026自动设置广播数据
 * @param       dev_name: 字符串参数，表示设备名称
 *              uuid : 字符串参数，标识符
 *              manufacturer_data : 十六进制字符串参数，制造商数据
 *              include_power : 若广播数据需包含 TX 功率，本参数应该设为 1；否则，为 0
 *              若未使用该命令设置广播参数，则使用默认广播参数
 * @retval      ATK_MB026_EOK  : 自动设置广播数据设置成功
 *              ATK_MB026_ERROR: 自动设置广播数据设置失败
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
 * @brief       ATK_MB026开始Bluetooth LE广播
 * @param       无
 * @retval      ATK_MB026_EOK  : Bluetooth LE广播成功
 *              ATK_MB026_ERROR: Bluetooth LE广播失败
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
 * @brief       ATK_MB026设置Bluetooth LE SPP参数
 * @param       cfg_enable:        0，重置所有 SPP 参数，后面参数无需填写
 *                                 1，后面参数需要填写
 *              tx_service_index : tx 服务序号
 *              tx_char_index :    tx 服务特征序号
 *              rx_service_index : rx 服务序号
 *              rx_char_index :    rx 服务特征序号
 *              auto_conn :        自动重连标志位，默认情况下，自动重连功能被使能
 *                                 0: 禁止 Bluetooth LE 透传自动重连功能。
 *                                 1: 使能 Bluetooth LE 透传自动重连功能
 * @retval      ATK_MB026_EOK  : Bluetooth LE SPP参数设置成功
 *              ATK_MB026_ERROR: Bluetooth LE SPP参数设置失败
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
 * @brief       ATK_MB026进入Bluetooth LE SPP模式
 * @param       无
 * @retval      ATK_MB026_EOK  : 进入Bluetooth LE SPP模式成功
 *              ATK_MB026_ERROR: 进入Bluetooth LE SPP模式失败
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
 * @brief       获取ATK-MB026连接状态
 * @param       无
 * @retval      ATK_MB026_CONNECTED   : 已连接
 *              ATK_MB026_DISCONNECTED: 未连接
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
 * @brief       设置ATK_MB026工作模式
 * @param       mode: 1，Station模式
 *                    2，AP模式
 *                    3，AP+Station模式
 * @retval      ATK_MB026_EOK   : 工作模式设置成功
 *              ATK_MB026_ERROR : 工作模式设置失败
 *              ATK_MB026_EINVAL: mode参数错误，工作模式设置失败
 */
uint8_t atk_mb026_set_mode(uint8_t mode)
{
    uint8_t ret;
    
    switch (mode)
    {
        case 1:
        {
            ret = atk_mb026_send_at_cmd("AT+CWMODE=1", "OK", 500);    /* Station模式 */
            break;
        }
        case 2:
        {
            ret = atk_mb026_send_at_cmd("AT+CWMODE=2", "OK", 500);    /* AP模式 */
            break;
        }
        case 3:
        {
            ret = atk_mb026_send_at_cmd("AT+CWMODE=3", "OK", 500);    /* AP+Station模式 */
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
 * @brief       ATK_MB026软件复位
 * @param       无
 * @retval      ATK_MB026_EOK  : 软件复位成功
 *              ATK_MB026_ERROR: 软件复位失败
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
 * @brief       ATK_MB026设置回显模式
 * @param       cfg: 0，关闭回显
 *                   1，打开回显
 * @retval      ATK_MB026_EOK  : 设置回显模式成功
 *              ATK_MB026_ERROR: 设置回显模式失败
 */
uint8_t atk_mb026_ate_config(uint8_t cfg)
{
    uint8_t ret;
    
    switch (cfg)
    {
        case 0:
        {
            ret = atk_mb026_send_at_cmd("ATE0", "OK", 500);   /* 关闭回显 */
            break;
        }
        case 1:
        {
            ret = atk_mb026_send_at_cmd("ATE1", "OK", 500);   /* 打开回显 */
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
 * @brief       ATK_MB026连接WIFI
 * @param       ssid: WIFI名称
 *              pwd : WIFI密码
 * @retval      ATK_MB026_EOK  : WIFI连接成功
 *              ATK_MB026_ERROR: WIFI连接失败
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
 * @brief       ATK_MB026获取IP地址
 * @param       buf: IP地址，需要16字节内存空间
 * @retval      ATK_MB026_EOK  : 获取IP地址成功
 *              ATK_MB026_ERROR: 获取IP地址失败
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
 * @brief       ATK_MB026连接TCP服务器
 * @param       server_ip  : TCP服务器IP地址
 *              server_port: TCP服务器端口号
 * @retval      ATK_MB026_EOK  : 连接TCP服务器成功
 *              ATK_MB026_ERROR: 连接TCP服务器失败
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
 * @brief       ATK_MB026进入透传
 * @param       无
 * @retval      ATK_MB026_EOK  : 进入透传成功
 *              ATK_MB026_ERROR: 进入透传失败
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
 * @brief       ATK_MB026退出透传
 * @param       无
 * @retval      无
 */
void atk_mb026_exit_unvarnished(void)
{
    atk_mb026_uart_printf("+++");
}

/**
 * @brief       ATK_MB026连接原子云服务器
 * @param       id : 原子云设备编号
 *              pwd: 原子云设备密码
 * @retval      ATK_MB026_EOK  : 连接原子云服务器成功
 *              ATK_MB026_ERROR: 连接原子云服务器失败
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
 * @brief       ATK_MB026连接新版原子云服务器
 * @param       id : 新版原子云设备编号
 *              pwd: 新版原子云设备密码
 * @retval      ATK_MB026_EOK  : 连接原子云服务器成功
 *              ATK_MB026_ERROR: 连接原子云服务器失败
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
 * @brief       ATK_MB026断开原子云服务器连接
 * @param       无
 * @retval      ATK_MB026_EOK  : 断开原子云服务器连接成功
 *              ATK_MB026_ERROR: 断开原子云服务器连接失败
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
