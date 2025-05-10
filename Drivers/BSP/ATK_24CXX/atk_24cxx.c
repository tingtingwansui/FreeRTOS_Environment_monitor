#include "./BSP/IIC/myiic.h"
#include "./BSP/ATK_24CXX/atk_24cxx.h"
#include "./SYSTEM/delay/delay.h"

// ѭ�������дָ��
uint16_t ee_write_index = 0;
uint16_t ee_read_index  = 0;

// �ⲿ����������ͬ����
extern float temperature;
extern float humidity;
extern float ppm;
extern int   fire;
extern SemaphoreHandle_t mutex_semphore_handle;

/**
 * @brief       ��ʼ��IIC�ӿ�
 * @param       ��
 * @retval      ��
 */
void atk_24cxx_init(void)
{
    iic_init();
}

/**
 * @brief       ��ATK_24CXXָ����ַ����һ������
 * @param       readaddr: ��ʼ�����ĵ�ַ
 * @retval      ����������
 */
uint8_t atk_24cxx_read_one_byte(uint16_t addr)
{
    uint8_t temp = 0;
    iic_start();                /* ������ʼ�ź� */

    /* ���ݲ�ͬ��24CXX�ͺ�, ���͸�λ��ַ
     * 1, 24C16���ϵ��ͺ�, ��2���ֽڷ��͵�ַ
     * 2, 24C16�����µ��ͺ�, ��1�����ֽڵ�ַ + ռ��������ַ��bit1~bit3λ ���ڱ�ʾ��λ��ַ, ���11λ��ַ
     *    ����24C01/02, ��������ַ��ʽ(8bit)Ϊ: 1  0  1  0  A2  A1  A0  R/W
     *    ����24C04,    ��������ַ��ʽ(8bit)Ϊ: 1  0  1  0  A2  A1  a8  R/W
     *    ����24C08,    ��������ַ��ʽ(8bit)Ϊ: 1  0  1  0  A2  a9  a8  R/W
     *    ����24C16,    ��������ַ��ʽ(8bit)Ϊ: 1  0  1  0  a10 a9  a8  R/W
     *    R/W      : ��/д����λ 0,��ʾд; 1,��ʾ��;
     *    A0/A1/A2 : ��Ӧ������1,2,3����(ֻ��24C01/02/04/8����Щ��)
     *    a8/a9/a10: ��Ӧ�洢���еĸ�λ��ַ, 11bit��ַ�����Ա�ʾ2048��λ��,����Ѱַ24C16�����ڵ��ͺ�
     */    
    if (EE_TYPE > AT24C16)      /* 24C16���ϵ��ͺ�, ��2���ֽڷ��͵�ַ */
    {
        iic_send_byte(0XA0);    /* ����д����, IIC�涨���λ��0, ��ʾд�� */
        iic_wait_ack();         /* ÿ�η�����һ���ֽ�,��Ҫ�ȴ�ACK */
        iic_send_byte(addr >> 8);/* ���͸��ֽڵ�ַ */
    }
    else 
    {
        iic_send_byte(0XA0 + ((addr >> 8) << 1));   /* �������� 0XA0 + ��λa8/a9/a10��ַ,д���� */
    }
    
    iic_wait_ack();             /* ÿ�η�����һ���ֽ�,��Ҫ�ȴ�ACK */
    iic_send_byte(addr % 256);  /* ���͵�λ��ַ */
    iic_wait_ack();             /* �ȴ�ACK, ��ʱ��ַ��������� */
    
    iic_start();                /* ���·�����ʼ�ź� */ 
    iic_send_byte(0XA1);        /* �������ģʽ, IIC�涨���λ��0, ��ʾ��ȡ */
    iic_wait_ack();             /* ÿ�η�����һ���ֽ�,��Ҫ�ȴ�ACK */
    temp = iic_read_byte(0);    /* ����һ���ֽ����� */
    iic_stop();                 /* ����һ��ֹͣ���� */
    return temp;
}

/**
 * @brief       ��ATK_24CXXָ����ַд��һ������
 * @param       addr: д�����ݵ�Ŀ�ĵ�ַ
 * @param       data: Ҫд�������
 * @retval      ��
 */
void atk_24cxx_write_one_byte(uint16_t addr, uint8_t data)
{
    /* ԭ��˵����:at24cxx_read_one_byte����, ��������ȫ���� */
    iic_start();                /* ������ʼ�ź� */

    if (EE_TYPE > AT24C16)      /* 24C16���ϵ��ͺ�, ��2���ֽڷ��͵�ַ */
    {
        iic_send_byte(0XA0);    /* ����д����, IIC�涨���λ��0, ��ʾд�� */
        iic_wait_ack();         /* ÿ�η�����һ���ֽ�,��Ҫ�ȴ�ACK */
        iic_send_byte(addr >> 8);/* ���͸��ֽڵ�ַ */
    }
    else 
    {
        iic_send_byte(0XA0 + ((addr >> 8) << 1));   /* �������� 0XA0 + ��λa8/a9/a10��ַ,д���� */
    }
    
    iic_wait_ack();             /* ÿ�η�����һ���ֽ�,��Ҫ�ȴ�ACK */
    iic_send_byte(addr % 256);  /* ���͵�λ��ַ */
    iic_wait_ack();             /* �ȴ�ACK, ��ʱ��ַ��������� */
    
    /* ��Ϊд���ݵ�ʱ��,����Ҫ�������ģʽ��,�������ﲻ�����·�����ʼ�ź��� */
    iic_send_byte(data);        /* ����1�ֽ� */
    iic_wait_ack();             /* �ȴ�ACK */
    iic_stop();                 /* ����һ��ֹͣ���� */
    delay_ms(10);               /* ע��: EEPROM д��Ƚ���,����ȵ�10ms����д��һ���ֽ� */
}
 
/**
 * @brief       ���ATK_24CXX�Ƿ�����
 *   @note      ���ԭ��: ��������ĩ��ַд��0X55, Ȼ���ٶ�ȡ, �����ȡֵΪ0X55
 *              ���ʾ�������. ����,���ʾ���ʧ��.
 *
 * @param       ��
 * @retval      �����
 *              0: ���ɹ�
 *              1: ���ʧ��
 */
uint8_t atk_24cxx_check(void)
{
    uint8_t temp;
    uint16_t addr = EE_TYPE;
    temp = atk_24cxx_read_one_byte(addr); /* ����ÿ�ο�����дAT24CXX */

    if (temp == 0X55)   /* ��ȡ�������� */
    {
        return 0;
    }
    else    /* �ų���һ�γ�ʼ������� */
    {
        atk_24cxx_write_one_byte(addr, 0X55); /* ��д������ */
        temp = atk_24cxx_read_one_byte(255);  /* �ٶ�ȡ���� */

        if (temp == 0X55)return 0;
    }

    return 1;
}

/**
 * @brief       ��ATK_24CXX�����ָ����ַ��ʼ����ָ������������
 * @param       addr    : ��ʼ�����ĵ�ַ ��24c02Ϊ0~255
 * @param       pbuf    : ���������׵�ַ
 * @param       datalen : Ҫ�������ݵĸ���
 * @retval      ��
 */
void atk_24cxx_read(uint16_t addr, uint8_t *pbuf, uint16_t datalen)
{
    while (datalen--)
    {
        *pbuf++ = atk_24cxx_read_one_byte(addr++);
    }
}

/**
 * @brief       ��ATK_24CXX�����ָ����ַ��ʼд��ָ������������
 * @param       addr    : ��ʼд��ĵ�ַ ��24c02Ϊ0~255
 * @param       pbuf    : ���������׵�ַ
 * @param       datalen : Ҫд�����ݵĸ���
 * @retval      ��
 */
void atk_24cxx_write(uint16_t addr, uint8_t *pbuf, uint16_t datalen)
{
    while (datalen--)
    {
        atk_24cxx_write_one_byte(addr, *pbuf);
        addr++;
        pbuf++;
    }
}

/**
 * @brief ��ʼ�� EEPROM ���棺�� EEPROM �лָ���дָ��
 */
void atk_24cxx_eeprom_init(void)
{
    ee_write_index = atk_24cxx_read_one_byte(EE_START_ADDR);
    ee_read_index  = atk_24cxx_read_one_byte(EE_START_ADDR + 1);
}

/**
 * @brief ���浱ǰ���������ݣ�ѭ��������ɼ�¼
 */
void buffer_current_reading(void)
{
    //printf("EEPROM�洢����,�¶ȣ�%.1f,ʪ�ȣ�%.1f,����������%.1f,����ǿ�ȣ�%d\r\n",temperature, humidity, ppm, fire);
    Record r = { temperature, humidity, ppm, fire };
    uint16_t addr = EE_START_ADDR + 2 + ee_write_index * EE_RECORD_SIZE;
    atk_24cxx_write(addr, (uint8_t *)&r, EE_RECORD_SIZE);

    ee_write_index = (ee_write_index + 1) % EE_MAX_RECORDS;
    if (ee_write_index == ee_read_index) {
        ee_read_index = (ee_read_index + 1) % EE_MAX_RECORDS;
    }
    // �־û�ָ��
    atk_24cxx_write_one_byte(EE_START_ADDR, (uint8_t)ee_write_index);
    atk_24cxx_write_one_byte(EE_START_ADDR + 1, (uint8_t)ee_read_index);
}

/**
 * @brief �ж��Ƿ��д��ϴ���������
 */
bool has_buffered_data(void)
{
    return (ee_read_index != ee_write_index);
}

/**
 * @brief ��ȡ��һ�������¼���ƽ���ָ��
 * @param[out] r Ŀ�� Record ָ��
 */
void read_next_record(Record *r)
{
    uint16_t addr = EE_START_ADDR + 2 + ee_read_index * EE_RECORD_SIZE;
    atk_24cxx_read(addr, (uint8_t *)r, EE_RECORD_SIZE);
    ee_read_index = (ee_read_index + 1) % EE_MAX_RECORDS;
}



