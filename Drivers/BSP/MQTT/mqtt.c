/* mqtt.c */
#include "./BSP/MQTT/mqtt.h"
#include "./BSP/ATK_MB026/atk_mb026.h"
#include <string.h>
#include <stdio.h>

bool is_connected = false;

// 客户端配置
static MQTT_Status MQTT_ConfigClient(const MQTT_Config* config) 
{
    char cmd[512];  // 扩大缓冲区应对长密码
    snprintf(cmd, sizeof(cmd),
        "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"",
        config->client_id, config->username, config->password);

    //printf("Sending config: %s\n", cmd);
    if (atk_mb026_send_at_cmd(cmd, "OK", 5000) != ATK_MB026_EOK) {
        return MQTT_CONFIGURE_FAILED;
    }
    return MQTT_OK;
}

// 连接
MQTT_Status MQTT_Connect(const MQTT_Config* config) 
{
    MQTT_Status ret = MQTT_ConfigClient(config);
    if (ret != MQTT_OK) return ret;
    
    char cmd[256];
    snprintf(cmd, sizeof(cmd), 
        "AT+MQTTCONN=0,\"%s\",%u,1\r\n",
        config->server_ip, config->port);

    //printf("Connecting: %s", cmd);
    if (atk_mb026_send_at_cmd(cmd, "+MQTTCONNECTED", 10000) != ATK_MB026_EOK) {
        is_connected = false;
        return MQTT_CONNECT_FAILED;
    }
    is_connected = true;
    return MQTT_OK;
}

// 订阅
MQTT_Status MQTT_Subscribe(const char* topic, uint8_t qos, uint32_t timeout_ms) 
{
    char cmd[256];
    snprintf(cmd, sizeof(cmd), 
        "AT+MQTTSUB=0,\"%s\",%u\r\n", 
        topic, qos);

    if (atk_mb026_send_at_cmd(cmd, "OK", timeout_ms) != ATK_MB026_EOK) {
        return MQTT_SUBSCRIBE_FAILED;
    }
    return MQTT_OK;
}

// 发布
MQTT_Status MQTT_Publish(const char* topic, const char* message, uint8_t retain, uint32_t timeout_ms) 
{
    char cmd[512];
    snprintf(cmd, sizeof(cmd), 
        "AT+MQTTPUB=0,\"%s\",\"%s\",0,%u\r\n",
        topic, message, retain);
    
    if (atk_mb026_send_at_cmd_task(cmd, "+MQTTPUBLISHED", timeout_ms) != ATK_MB026_EOK) {
        return MQTT_PUBLISH_FAILED;
    }
    return MQTT_OK;
}

bool MQTT_IsConnected(void) {
    return is_connected;
}

void MQTT_Disconnect(void) {
    if (is_connected) {
        atk_mb026_send_at_cmd("AT+MQTTCLEAN=0\r\n", "OK", 1000);
        is_connected = false;
    }
}

/**
 * @brief 发送 MQTT 心跳包（PINGREQ）
 * @retval MQTT_OK 成功，其他 失败
 */
MQTT_Status MQTT_Ping(void) {
    // AT+MQTTPING=0\r\n
    char *cmd = "AT+MQTTPING=0\r\n";
    // 等待 +MQTTPINGRESP
    if (atk_mb026_send_at_cmd(cmd, "+MQTTPINGRESP", 5000) != ATK_MB026_EOK) {
        is_connected = false;
        return MQTT_PING_FAILED;
    }
    return MQTT_OK;
}

/**
 * @brief 处理底层 AT 接口，接收并解析服务器下行（可选）
 */
void MQTT_Yield(void) {
    uint8_t *p = atk_mb026_uart_rx_get_frame();
    if (p) {
        // 检查下行订阅消息
        if (strstr((char*)p, "+MQTTSUBRECV:")) {
            char *json = strchr((char*)p, '{');
            if (json) {
                printf("[MQTT_RX] %s\r\n", json);
            }
        }
        atk_mb026_uart_rx_restart();
    }
}

