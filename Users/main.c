//��������
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include <string.h>
//ģ������
#include "./BSP/LED/led.h"//led
#include "./BSP/ADC/adc.h"//adc
#include "./BSP/ATK_BUZZER/atk_buzzer.h"//������
#include "./BSP/ATK_OLED/atk_oled.h"//OLED
#include "./BSP/ATK_AHT20/atk_aht20.h"//��ʪ��
#include "./BSP/ATK_AIR/atk_air.h"//��������������
#include "./BSP/ATK_FIRE/atk_fire.h"//���洫����
#include "./BSP/ATK_24CXX/atk_24cxx.h"//eeprom
#include "./BSP/KEY/key.h"//KEY
#include "./BSP/ATK_FAN/atk_fan.h"//����ģ��
#include "./BSP/ATK_MB026/atk_mb026.h"//ESP8266
#include "./BSP/MQTT/mqtt.h"//MQTT
//FreeRTOS����
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* START��������
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define START_TASK_PRIO 1                                   /* �������ȼ� */
#define START_STK_SIZE  128                                 /* �����ջ��С */
TaskHandle_t StartTask_Handler;                             /* ������ */
void start_task(void *pvParameters);                        /* ������ */

/* OLED_DISPLAY��������
 * ����������OLED��ʾ
 */
#define OLED_DISPLAY_TASK_PRIO 2                                   /* �������ȼ� */
#define OLED_DISPLAY_STK_SIZE  256                                 /* �����ջ��С */
TaskHandle_t OLED_DISPLAY_Task_Handler;                             /* ������ */
void oled_display_task(void *pvParameters);                        /* ������ */

/* EEPROM��������
 * ����������OLED��ʾ
 */
#define EEPROM_TASK_PRIO 3                                   /* �������ȼ� */
#define EEPROM_STK_SIZE  256                                 /* �����ջ��С */
TaskHandle_t EEPROM_Task_Handler;                             /* ������ */
void eeprom_task(void *pvParameters);                        /* ������ */

/* MQTT_UPLOAD��������
 * ���������ڸ����Ʒ���������
 */
#define MQTT_UPLOAD_TASK_PRIO 4                                   /* �������ȼ� */
#define MQTT_UPLOAD_STK_SIZE  512                                 /* �����ջ��С */
TaskHandle_t MQTT_UPLOAD_Task_Handler;                             /* ������ */
void mqtt_upload_task(void *pvParameters);                        /* ������ */

/* DATA_ACQUISITION��������
 * ����������ADC�ɼ������棬��������
 */
#define DATA_ACQUISITION_TASK_PRIO 5                                   /* �������ȼ� */
#define DATA_ACQUISITION_STK_SIZE  512                                 /* �����ջ��С */
TaskHandle_t DATA_ACQUISITION_Task_Handler;                             /* ������ */
void data_acquisition_task(void *pvParameters);                        /* ������ */

/* EVENT_PROCESS��������
 * ���������ڴ������������
 */
#define EVENT_PROCESS_TASK_PRIO 6                                   /* �������ȼ� */
#define EVENT_PROCESS_STK_SIZE  128                                 /* �����ջ��С */
TaskHandle_t EVENT_PROCESS_Task_Handler;                             /* ������ */
void event_process_task(void *pvParameters);                        /* ������ */


//MQTTģ������
#define DEMO_WIFI_SSID          "user"                 //WIFI����
#define DEMO_WIFI_PWD           "abcdefgh"             //WIFI����
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
//�����ģ������
#define MAX_SPEED   30          /* ���ƱȽ�ֵ ������ٶ� */
//EEPROMģ������
/* Ҫд�뵽24c02���ַ������� */
const uint8_t g_text_buf[] = {"STM32 IIC TEST"};
#define TEXT_SIZE       sizeof(g_text_buf)  /* TEXT�ַ������� */
//�¶ȱ�������
float temperature=0;        /* ʪ�� */
float humidity=0;           /* ʪ�� */
float ppm=0;                /* �к�����ƽ��Ũ�� */
int fire=0;                 /* ����ǿ��ֵ */

//Ϊ�˷�ֹ���ݲɼ��������ϴ���ͻ
QueueHandle_t mutex_semphore_handle;
//��Ϣ����
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
//���ݻ���������ʾ����״��
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
//�����ж������Ƿ�����
static bool network_connected(void)
{
    char ipbuf[16];
    // ͨ����ȡ IP �ж� WiFi �����Ƿ�����
    if (atk_mb026_get_ip(ipbuf) == 0) {
        return true;
    }
    return false;
}
int main(void)
{
    uint8_t ret;
    char ip_buf[16];
    
    HAL_Init();                                             /* ��ʼ��HAL�� */
    sys_stm32_clock_init(RCC_PLL_MUL9);                     /* ����ʱ��, 72Mhz */
    delay_init(72);                                         /* ��ʱ��ʼ�� */
    usart_init(115200);                                     /* ��ʼ������ */
    key_init();                                 /* ��ʼ������ */
    led_init();                                  /* led��ʼ�� */
    atk_24cxx_init();                           /* ��ʼ��24CXX */
    while (atk_24cxx_check())                   /* ��ⲻ��24c02 */
    {
        printf("24C02 Check Failed!\r\n");
        delay_ms(500);
    }
    atk_oled_init();                            /* ��ʼ��OLED */
    
    while(atk_aht20_init())                                                 /* ��ʼ��ʪ�ȴ����� */
    {
        printf("AHT20��������ʼ��ʧ��\r\n");
        delay_ms(200);
    }
    atk_air_init();                                                         /* ��ʼ����������������ģ�� */
    atk_fire_init();                                                        /* ��ʼ�����洫���� */
    
    atk_buzzer_init();          /* ��ʼ��������ģ�� */
    atk_fan_init();             /* ��ʼ������ģ�� */
    atk_24cxx_eeprom_init();    /* ��ʼ������ָ��(EEPROM) */
    
    /* ��ʼ��ATK-MB026 */
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

    /* ����MQTT����������ҳ6����ҳ7������*/
    while(1){
        ret = MQTT_Connect(&mqtt_default_config);
        if(ret == MQTT_OK) break;
        printf("MQTT Connect Success! Code:%d\r\n", ret);
    }
    
    /* �������⣨��ҳ7����������*/
    if (MQTT_Subscribe("$sys/awWUvMdMC1/test/thing/property/post/reply", 1, 2000) == MQTT_OK) {
        printf("Subscribed to reply topic\r\n");
    }
    
    mutex_semphore_handle=xSemaphoreCreateMutex();
    if(mutex_semphore_handle!=NULL)
    {
      printf("�����ź��������ɹ�\r\n");
    }
    
    printf("��ʼ�����\r\n");
    
    xTaskCreate((TaskFunction_t )start_task,                /* ������ */
                (const char*    )"start_task",              /* �������� */
                (uint16_t       )START_STK_SIZE,            /* �����ջ��С */
                (void*          )NULL,                      /* ������������Ĳ��� */
                (UBaseType_t    )START_TASK_PRIO,           /* �������ȼ� */
                (TaskHandle_t*  )&StartTask_Handler);       /* ������ */
                
    vTaskStartScheduler();
}

/**
 * @brief       start_task
 * @param       pvParameters: �������(δ�õ�)
 * @retval      ��
 */
void start_task(void *pvParameters)
{
    ( void ) pvParameters;
    taskENTER_CRITICAL();                                   /* �����ٽ��� */
    
    /* ����OLED_DISPLAY���� */
    xTaskCreate((TaskFunction_t )oled_display_task,
                (const char*    )"oled_display_task",
                (uint16_t       )OLED_DISPLAY_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )OLED_DISPLAY_TASK_PRIO,
                (TaskHandle_t*  )&OLED_DISPLAY_Task_Handler);
                
    /* ����EEPROM���� */
    xTaskCreate((TaskFunction_t )eeprom_task,
                (const char*    )"eeprom_task",
                (uint16_t       )EEPROM_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )EEPROM_TASK_PRIO,
                (TaskHandle_t*  )&EEPROM_Task_Handler);
                
    /* ����MQTT_UPLOAD���� */
    xTaskCreate((TaskFunction_t )mqtt_upload_task,
                (const char*    )"mqtt_upload_task",
                (uint16_t       )MQTT_UPLOAD_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )MQTT_UPLOAD_TASK_PRIO,
                (TaskHandle_t*  )&MQTT_UPLOAD_Task_Handler);
                
    /* ����DATA_ACQUISITION���� */
    xTaskCreate((TaskFunction_t )data_acquisition_task,
                (const char*    )"data_acquisition_task",
                (uint16_t       )DATA_ACQUISITION_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )DATA_ACQUISITION_TASK_PRIO,
                (TaskHandle_t*  )&DATA_ACQUISITION_Task_Handler);
                
    /* ����EVENT_PROCESS���� */
    xTaskCreate((TaskFunction_t )event_process_task,
                (const char*    )"event_process_task",
                (uint16_t       )EVENT_PROCESS_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )EVENT_PROCESS_TASK_PRIO,
                (TaskHandle_t*  )&EVENT_PROCESS_Task_Handler);
                
    vTaskDelete(StartTask_Handler);                     /* ɾ����ʼ���� */
    taskEXIT_CRITICAL();                                /* �˳��ٽ��� */
}

/**
 * @brief       OLED_DISPLAY����
 * @param       pvParameters: �������(δ�õ�)
 * @retval      ��
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
 * @brief       EEPROM����
 * @param       pvParameters: �������(δ�õ�)
 * @retval      ��
 */
void eeprom_task(void *pvParameters)
{
    Record rec;
    uint8_t t=0;
    while(1)
    {
        if (!network_connected() || is_connected==false) {
            printf("�Ͽ����ӣ��洢һ������,%u\r\n",t);
            // ������ MQTT δ����ʱ����������
            if(++t==5)
            {
              buffer_current_reading();
              t=0;
            }
        } else {
            // ������ʱ��ѭ���ϴ����л���
            while (has_buffered_data()==true) {
                read_next_record(&rec);
                printf("�����ϴ�����\r\n");
                // ��ԭ��ʽ�������ϴ�
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
                printf("�ϴ��������\r\n");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1s ����
    }
}

/**
 * @brief       MQTT_UPLOAD����
 * @param       pvParameters: �������(δ�õ�)
 * @retval      ��
 */
void mqtt_upload_task(void *pvParameters)
{
    while (1) {
        if (!MQTT_IsConnected()) {
            printf("��������\r\n");
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
 * @brief       DATA_ACQUISITION����
 * @param       pvParameters: �������(δ�õ�)
 * @retval      ��
 */
void data_acquisition_task(void *pvParameters)
{
    while(1)
    {
        if (xSemaphoreTake(mutex_semphore_handle, portMAX_DELAY) == pdTRUE) {
            atk_aht20_read_data(&temperature, &humidity);       /* ��ȡATH20���������� */
            ppm = atk_air_get_ppm();    /* ��Ϻ��������ppm */
            fire=(int)atk_fire_get_val();
            xSemaphoreGive(mutex_semphore_handle);
        }
        printf("�¶�: %.2f��\r\n", temperature);            //-20.0~100.0
        printf("ʪ��: %.2f%%\r\n", humidity);              //0-200
        printf("�к������ƽ��Ũ�ȣ�%0.3fppm\r\n", ppm);   //0~2000
        printf("����ǿ��ֵ��%d%%\r\n", fire);              
        printf("\r\n");
        vTaskDelay(1000);
    }
}

/**
 * @brief       EEPROM����
 * @param       pvParameters: �������(δ�õ�)
 * @retval      ��
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
