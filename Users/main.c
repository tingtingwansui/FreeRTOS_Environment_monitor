//基础配置
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include <string.h>
//模块配置
#include "./BSP/LED/led.h"//led
#include "./BSP/ADC/adc.h"//adc
#include "./BSP/ATK_BUZZER/atk_buzzer.h"//蜂鸣器
#include "./BSP/ATK_OLED/atk_oled.h"//OLED
#include "./BSP/ATK_AHT20/atk_aht20.h"//温湿度
#include "./BSP/ATK_AIR/atk_air.h"//空气质量传感器
#include "./BSP/ATK_FIRE/atk_fire.h"//火焰传感器
#include "./BSP/ATK_24CXX/atk_24cxx.h"//eeprom
#include "./BSP/KEY/key.h"//KEY
#include "./BSP/ATK_FAN/atk_fan.h"//风扇模块
#include "./BSP/ATK_MB026/atk_mb026.h"//ESP8266
#include "./BSP/MQTT/mqtt.h"//MQTT
//FreeRTOS配置
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* START任务配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO 1                                   /* 任务优先级 */
#define START_STK_SIZE  128                                 /* 任务堆栈大小 */
TaskHandle_t StartTask_Handler;                             /* 任务句柄 */
void start_task(void *pvParameters);                        /* 任务函数 */

/* OLED_DISPLAY任务配置
 * 此任务用于OLED显示
 */
#define OLED_DISPLAY_TASK_PRIO 2                                   /* 任务优先级 */
#define OLED_DISPLAY_STK_SIZE  256                                 /* 任务堆栈大小 */
TaskHandle_t OLED_DISPLAY_Task_Handler;                             /* 任务句柄 */
void oled_display_task(void *pvParameters);                        /* 任务函数 */

/* EEPROM任务配置
 * 此任务用于OLED显示
 */
#define EEPROM_TASK_PRIO 3                                   /* 任务优先级 */
#define EEPROM_STK_SIZE  256                                 /* 任务堆栈大小 */
TaskHandle_t EEPROM_Task_Handler;                             /* 任务句柄 */
void eeprom_task(void *pvParameters);                        /* 任务函数 */

/* MQTT_UPLOAD任务配置
 * 此任务用于更新云服务器数据
 */
#define MQTT_UPLOAD_TASK_PRIO 4                                   /* 任务优先级 */
#define MQTT_UPLOAD_STK_SIZE  512                                 /* 任务堆栈大小 */
TaskHandle_t MQTT_UPLOAD_Task_Handler;                             /* 任务句柄 */
void mqtt_upload_task(void *pvParameters);                        /* 任务函数 */

/* DATA_ACQUISITION任务配置
 * 此任务用于ADC采集，火焰，空气质量
 */
#define DATA_ACQUISITION_TASK_PRIO 5                                   /* 任务优先级 */
#define DATA_ACQUISITION_STK_SIZE  512                                 /* 任务堆栈大小 */
TaskHandle_t DATA_ACQUISITION_Task_Handler;                             /* 任务句柄 */
void data_acquisition_task(void *pvParameters);                        /* 任务函数 */

/* EVENT_PROCESS任务配置
 * 此任务用于处理环境特殊情况
 */
#define EVENT_PROCESS_TASK_PRIO 6                                   /* 任务优先级 */
#define EVENT_PROCESS_STK_SIZE  128                                 /* 任务堆栈大小 */
TaskHandle_t EVENT_PROCESS_Task_Handler;                             /* 任务句柄 */
void event_process_task(void *pvParameters);                        /* 任务函数 */


//MQTT模块配置
#define DEMO_WIFI_SSID          "user"                 //WIFI名称
#define DEMO_WIFI_PWD           "abcdefgh"             //WIFI密码
const MQTT_Config mqtt_default_config = {
    .server_ip = "mqtts.heclouds.com",
    .port = 1883,
    .client_id = "test",
    .username = "awWUvMdMC1",
    .password = "version=2018-10-31&res=products%2FawWUvMdMC1%2Fdevices%2Ftest&et=2810351320&method=md5&sign=SZQTfZaZqYxY%2BQw%2F%2FqWNsg%3D%3D"
};
extern bool is_connected;
static char payload_part1[128];
static char payload_part2[128];
//电风扇模块配置
#define MAX_SPEED   30          /* 限制比较值 即最大速度 */
//EEPROM模块配置
/* 要写入到24c02的字符串数组 */
const uint8_t g_text_buf[] = {"STM32 IIC TEST"};
#define TEXT_SIZE       sizeof(g_text_buf)  /* TEXT字符串长度 */
//温度变量设置
float temperature=0;        /* 湿度 */
float humidity=0;           /* 湿度 */
float ppm=0;                /* 有害气体平均浓度 */
int fire=0;                 /* 火焰强度值 */

//为了防止数据采集和数据上传冲突
QueueHandle_t mutex_semphore_handle;
//消息处理
static void demo_upload_data(void) {
    uint8_t *buf = atk_mb026_uart_rx_get_frame();
    if (buf != NULL) {
        if (strstr((char*)buf, "+MQTTSUBRECV:") != NULL) {
            char *payload_start = strchr((char*)buf, '{');
            if (payload_start) {
                printf("[MQTT] Received: %s\r\n", payload_start);
            }
        }
        atk_mb026_uart_rx_restart();
    }
}
//根据环境数据显示天气状况
char* get_weather_status(float temp, float hum, float ppm)
{
    if (ppm > 1000) return "Poor air";
    if (hum > 80) return "Humid   ";
    if (hum < 30) return "Dry     ";
    if (temp > 25) return "Hot     ";
    if (temp < 10) return "Cold    ";
    if (temp >= 10 && temp <= 25 && hum >= 30 && hum <= 70 && ppm < 500) return "Good    ";
    return "Unknown";
}
//用来判断网络是否连接
static bool network_connected(void)
{
    char ipbuf[16];
    // 通过获取 IP 判断 WiFi 连接是否正常
    if (atk_mb026_get_ip(ipbuf) == 0) {
        return true;
    }
    return false;
}
int main(void)
{
    uint8_t ret;
    char ip_buf[16];
    
    HAL_Init();                                             /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9);                     /* 设置时钟, 72Mhz */
    delay_init(72);                                         /* 延时初始化 */
    usart_init(115200);                                     /* 初始化串口 */
    key_init();                                 /* 初始化按键 */
    led_init();                                  /* led初始化 */
    atk_24cxx_init();                           /* 初始化24CXX */
    while (atk_24cxx_check())                   /* 检测不到24c02 */
    {
        printf("24C02 Check Failed!\r\n");
        delay_ms(500);
    }
    atk_oled_init();                            /* 初始化OLED */
    
    while(atk_aht20_init())                                                 /* 初始温湿度传感器 */
    {
        printf("AHT20传感器初始化失败\r\n");
        delay_ms(200);
    }
    atk_air_init();                                                         /* 初始化空气质量传感器模块 */
    atk_fire_init();                                                        /* 初始化火焰传感器 */
    
    atk_buzzer_init();          /* 初始化蜂鸣器模块 */
    atk_fan_init();             /* 初始化风扇模块 */
    atk_24cxx_eeprom_init();    /* 初始化缓存指针(EEPROM) */
    
    /* 初始化ATK-MB026 */
    while(1)
    {
      ret = atk_mb026_init(115200);
      if (ret != 0) {
          printf("ATK-MB026 init failed!\r\n");
          delay_ms(200);
      }
      else{
          printf("ATK-MB026 init success!\r\n");
          break;
      }
    }
    
    printf("Joining to AP...\r\n");
    while(1)
    {
      ret  = atk_mb026_restore();
      delay_ms(1000);
      ret += atk_mb026_at_test();
      ret += atk_mb026_set_mode(1);
      ret += atk_mb026_sw_reset();
      delay_ms(500);
      ret += atk_mb026_ate_config(0);
      ret += atk_mb026_join_ap(DEMO_WIFI_SSID, DEMO_WIFI_PWD);
      ret += atk_mb026_get_ip(ip_buf);
      
      if (ret != 0) {
          printf("Connect WiFi failed!\r\n");
          delay_ms(200);
      }
      else
      {
          printf("Connect WiFi Success!\r\n");
          break;
      }
    }

    /* 连接MQTT服务器（网页6、网页7调整）*/
    while(1){
        ret = MQTT_Connect(&mqtt_default_config);
        if(ret == MQTT_OK) break;
        printf("MQTT Connect Success! Code:%d\r\n", ret);
    }
    
    /* 订阅主题（网页7参数调整）*/
    if (MQTT_Subscribe("$sys/awWUvMdMC1/test/thing/property/post/reply", 1, 2000) == MQTT_OK) {
        printf("Subscribed to reply topic\r\n");
    }
    
    mutex_semphore_handle=xSemaphoreCreateMutex();
    if(mutex_semphore_handle!=NULL)
    {
      printf("互斥信号量创建成功\r\n");
    }
    
    printf("初始化完毕\r\n");
    
    xTaskCreate((TaskFunction_t )start_task,                /* 任务函数 */
                (const char*    )"start_task",              /* 任务名称 */
                (uint16_t       )START_STK_SIZE,            /* 任务堆栈大小 */
                (void*          )NULL,                      /* 传入给任务函数的参数 */
                (UBaseType_t    )START_TASK_PRIO,           /* 任务优先级 */
                (TaskHandle_t*  )&StartTask_Handler);       /* 任务句柄 */
                
    vTaskStartScheduler();
}

/**
 * @brief       start_task
 * @param       pvParameters: 传入参数(未用到)
 * @retval      无
 */
void start_task(void *pvParameters)
{
    ( void ) pvParameters;
    taskENTER_CRITICAL();                                   /* 进入临界区 */
    
    /* 创建OLED_DISPLAY任务 */
    xTaskCreate((TaskFunction_t )oled_display_task,
                (const char*    )"oled_display_task",
                (uint16_t       )OLED_DISPLAY_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )OLED_DISPLAY_TASK_PRIO,
                (TaskHandle_t*  )&OLED_DISPLAY_Task_Handler);
                
    /* 创建EEPROM任务 */
    xTaskCreate((TaskFunction_t )eeprom_task,
                (const char*    )"eeprom_task",
                (uint16_t       )EEPROM_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )EEPROM_TASK_PRIO,
                (TaskHandle_t*  )&EEPROM_Task_Handler);
                
    /* 创建MQTT_UPLOAD任务 */
    xTaskCreate((TaskFunction_t )mqtt_upload_task,
                (const char*    )"mqtt_upload_task",
                (uint16_t       )MQTT_UPLOAD_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )MQTT_UPLOAD_TASK_PRIO,
                (TaskHandle_t*  )&MQTT_UPLOAD_Task_Handler);
                
    /* 创建DATA_ACQUISITION任务 */
    xTaskCreate((TaskFunction_t )data_acquisition_task,
                (const char*    )"data_acquisition_task",
                (uint16_t       )DATA_ACQUISITION_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )DATA_ACQUISITION_TASK_PRIO,
                (TaskHandle_t*  )&DATA_ACQUISITION_Task_Handler);
                
    /* 创建EVENT_PROCESS任务 */
    xTaskCreate((TaskFunction_t )event_process_task,
                (const char*    )"event_process_task",
                (uint16_t       )EVENT_PROCESS_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )EVENT_PROCESS_TASK_PRIO,
                (TaskHandle_t*  )&EVENT_PROCESS_Task_Handler);
                
    vTaskDelete(StartTask_Handler);                     /* 删除开始任务 */
    taskEXIT_CRITICAL();                                /* 退出临界区 */
}

/**
 * @brief       OLED_DISPLAY任务
 * @param       pvParameters: 传入参数(未用到)
 * @retval      无
 */
void oled_display_task(void *pvParameters)
{
    char buf[32];
    
    while(1)
    {
      char* weather = get_weather_status(temperature, humidity, ppm);
      snprintf(buf, sizeof(buf), "TEMP: %.1fC", temperature);
      atk_oled_show_string(0, 0, buf, 12);
      
      snprintf(buf, sizeof(buf), "HUMI: %.1f%%", humidity);
      atk_oled_show_string(0, 12, buf, 12);
      
      snprintf(buf, sizeof(buf), "PPM: %.0f", ppm);
      atk_oled_show_string(0, 24, buf, 12);
      
      atk_oled_show_string(0, 36, "STATE:", 12);
      atk_oled_show_string(64, 36, weather, 12);

      atk_oled_refresh_gram();
      vTaskDelay(2000);
    }
}

/**
 * @brief       EEPROM任务
 * @param       pvParameters: 传入参数(未用到)
 * @retval      无
 */
void eeprom_task(void *pvParameters)
{
    Record rec;
    uint8_t t=0;
    while(1)
    {
        if (!network_connected() || is_connected==false) {
            printf("断开连接，存储一次数据,%u\r\n",t);
            // 断网或 MQTT 未连接时，缓存数据
            if(++t==5)
            {
              buffer_current_reading();
              t=0;
            }
        } else {
            // 已连接时，循环上传所有缓存
            while (has_buffered_data()==true) {
                read_next_record(&rec);
                printf("正在上传数据\r\n");
                // 按原格式分两段上传
                char payload1[128];
                snprintf(payload1, sizeof(payload1),
                    "{\\\"id\\\":\\\"1005\\\"\\,\\\"version\\\":\\\"1.0\\\"\\,\\\"params\\\":{"
                    "\\\"ppm\\\":{\\\"value\\\":%.1f}\\,"
                    "\\\"fire\\\":{\\\"value\\\":%d}"
                    "}}",
                    rec.ppm, rec.fire);
                MQTT_Publish("$sys/awWUvMdMC1/test/thing/property/post", payload1, 0, 1000);

                char payload2[128];
                snprintf(payload2, sizeof(payload2),
                    "{\\\"id\\\":\\\"1005\\\"\\,\\\"version\\\":\\\"1.0\\\"\\,\\\"params\\\":{"
                    "\\\"temp\\\":{\\\"value\\\":%.1f}\\,"
                    "\\\"hum\\\":{\\\"value\\\":%.1f}"
                    "}}",
                    rec.temperature, rec.humidity);
                MQTT_Publish("$sys/awWUvMdMC1/test/thing/property/post", payload2, 0, 1000);
                printf("上传数据完成\r\n");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1s 周期
    }
}

/**
 * @brief       MQTT_UPLOAD任务
 * @param       pvParameters: 传入参数(未用到)
 * @retval      无
 */
void mqtt_upload_task(void *pvParameters)
{
    while (1) {
        if (!MQTT_IsConnected()) {
            printf("重新连接\r\n");
            vTaskDelay(pdMS_TO_TICKS(3000));
            MQTT_Connect(&mqtt_default_config);
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
        if (xSemaphoreTake(mutex_semphore_handle, portMAX_DELAY) == pdTRUE) {
            snprintf(payload_part1, sizeof(payload_part1), 
                "{\\\"id\\\":\\\"1005\\\"\\,\\\"version\\\":\\\"1.0\\\"\\,\\\"params\\\":{"
                "\\\"ppm\\\":{\\\"value\\\":%.1f}\\,"
                "\\\"fire\\\":{\\\"value\\\":%d}"
                "}}",
                ppm,fire);
            snprintf(payload_part2, sizeof(payload_part2), 
                "{\\\"id\\\":\\\"1005\\\"\\,\\\"version\\\":\\\"1.0\\\"\\,\\\"params\\\":{"
                "\\\"temp\\\":{\\\"value\\\":%.1f}\\,"
                "\\\"hum\\\":{\\\"value\\\":%.1f}"
                "}}",
                temperature, humidity);
            xSemaphoreGive(mutex_semphore_handle);
        }
        MQTT_Publish("$sys/awWUvMdMC1/test/thing/property/post",payload_part1, 0,1000);
        MQTT_Publish("$sys/awWUvMdMC1/test/thing/property/post",payload_part2, 0,1000);
        
        if (MQTT_Ping() != MQTT_OK) {
            printf("Ping fail, will reconnect\r\n");
            is_connected = false;
        }
        
        demo_upload_data();
        vTaskDelay(5000);
    }
}

/**
 * @brief       DATA_ACQUISITION任务
 * @param       pvParameters: 传入参数(未用到)
 * @retval      无
 */
void data_acquisition_task(void *pvParameters)
{
    while(1)
    {
        if (xSemaphoreTake(mutex_semphore_handle, portMAX_DELAY) == pdTRUE) {
            atk_aht20_read_data(&temperature, &humidity);       /* 读取ATH20传感器数据 */
            ppm = atk_air_get_ppm();    /* 拟合函数换算出ppm */
            fire=(int)atk_fire_get_val();
            xSemaphoreGive(mutex_semphore_handle);
        }
        printf("温度: %.2f℃\r\n", temperature);            //-20.0~100.0
        printf("湿度: %.2f%%\r\n", humidity);              //0-200
        printf("有害气体的平均浓度：%0.3fppm\r\n", ppm);   //0~2000
        printf("火焰强度值：%d%%\r\n", fire);              
        printf("\r\n");
        vTaskDelay(1000);
    }
}

/**
 * @brief       EEPROM任务
 * @param       pvParameters: 传入参数(未用到)
 * @retval      无
 */
void event_process_task(void *pvParameters)
{
    while(1)
    {
        if (temperature > 30.0) atk_fan_pwm_set(MAX_SPEED);
        else atk_fan_pwm_set(0);
        if (fire > 90) ATK_BUZZER(1);
        else ATK_BUZZER(0);
        vTaskDelay(1000);
    }
}
