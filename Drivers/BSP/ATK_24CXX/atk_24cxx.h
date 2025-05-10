#ifndef __ATK_24CXX_H
#define __ATK_24CXX_H

#include "./SYSTEM/sys/sys.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <string.h>
#include <stdbool.h>

#define EE_START_ADDR    0             // EEPROM ��ʼ��ַ�����ڴ��ָ��
#define EE_MAX_RECORDS   50            // ��󻺴���Ŀ��
#define EE_RECORD_SIZE   sizeof(Record)

typedef struct {
    float temperature;
    float humidity;
    float ppm;
    int   fire;
} Record;

#define AT24C01     127
#define AT24C02     255
#define AT24C04     511
#define AT24C08     1023
#define AT24C16     2047
#define AT24C32     4095
#define AT24C64     8191
#define AT24C128    16383
#define AT24C256    32767

/* ������ʹ�õ���24c02�����Զ���EE_TYPEΪAT24C02 */

#define EE_TYPE     AT24C02

void atk_24cxx_init(void);        /* ��ʼ��IIC */
uint8_t atk_24cxx_check(void);    /* ������� */
uint8_t atk_24cxx_read_one_byte(uint16_t addr);                       /* ָ����ַ��ȡһ���ֽ� */
void atk_24cxx_write_one_byte(uint16_t addr,uint8_t data);            /* ָ����ַд��һ���ֽ� */
void atk_24cxx_write(uint16_t addr, uint8_t *pbuf, uint16_t datalen); /* ��ָ����ַ��ʼд��ָ�����ȵ����� */
void atk_24cxx_read(uint16_t addr, uint8_t *pbuf, uint16_t datalen);  /* ��ָ����ַ��ʼ����ָ�����ȵ����� */

// ˽�нӿ�
void atk_24cxx_eeprom_init(void);
void buffer_current_reading(void);
bool has_buffered_data(void);
void read_next_record(Record *r);

#endif













