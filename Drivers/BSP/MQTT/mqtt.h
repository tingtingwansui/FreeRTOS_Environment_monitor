/* mqtt.h */
#ifndef MQTT_H
#define MQTT_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    const char* server_ip;
    uint16_t port;
    const char* client_id;
    const char* username;
    const char* password;
} MQTT_Config;

// ¸ù¾ÝÍøÒ³7¡¢ÍøÒ³8À©Õ¹´íÎóÂë
typedef enum {
    MQTT_OK = 0,
    MQTT_CONFIGURE_FAILED,
    MQTT_CONNECT_FAILED,
    MQTT_SUBSCRIBE_FAILED,
    MQTT_PUBLISH_FAILED,
    MQTT_INVALID_PARAM,
    MQTT_PING_FAILED
} MQTT_Status;

extern const MQTT_Config mqtt_default_config;

MQTT_Status MQTT_Connect(const MQTT_Config* config);
MQTT_Status MQTT_Subscribe(const char* topic, uint8_t qos, uint32_t timeout_ms);
MQTT_Status MQTT_Publish(const char* topic, const char* message, uint8_t retain, uint32_t timeout_ms);
bool MQTT_IsConnected(void);
void MQTT_Disconnect(void);
MQTT_Status MQTT_Ping(void);
void MQTT_Yield(void);

#endif
