#include "./BSP/IIC/myiic.h"
#include "./BSP/ATK_24CXX/atk_24cxx.h"
#include "./SYSTEM/delay/delay.h"

// 循环缓冲读写指针
uint16_t ee_write_index = 0;
uint16_t ee_read_index  = 0;

// 外部共享数据与同步锁
extern float temperature;
extern float humidity;
extern float ppm;
extern int   fire;
extern SemaphoreHandle_t mutex_semphore_handle;

/**
 * @brief       初始化IIC接口
 * @param       无
 * @retval      无
 */
void atk_24cxx_init(void)
{
    iic_init();
}

/**
 * @brief       在ATK_24CXX指定地址读出一个数据
 * @param       readaddr: 开始读数的地址
 * @retval      读到的数据
 */
uint8_t atk_24cxx_read_one_byte(uint16_t addr)
{
    uint8_t temp = 0;
    iic_start();                /* 发送起始信号 */

    /* 根据不同的24CXX型号, 发送高位地址
     * 1, 24C16以上的型号, 分2个字节发送地址
     * 2, 24C16及以下的型号, 分1个低字节地址 + 占用器件地址的bit1~bit3位 用于表示高位地址, 最多11位地址
     *    对于24C01/02, 其器件地址格式(8bit)为: 1  0  1  0  A2  A1  A0  R/W
     *    对于24C04,    其器件地址格式(8bit)为: 1  0  1  0  A2  A1  a8  R/W
     *    对于24C08,    其器件地址格式(8bit)为: 1  0  1  0  A2  a9  a8  R/W
     *    对于24C16,    其器件地址格式(8bit)为: 1  0  1  0  a10 a9  a8  R/W
     *    R/W      : 读/写控制位 0,表示写; 1,表示读;
     *    A0/A1/A2 : 对应器件的1,2,3引脚(只有24C01/02/04/8有这些脚)
     *    a8/a9/a10: 对应存储整列的高位地址, 11bit地址最多可以表示2048个位置,可以寻址24C16及以内的型号
     */    
    if (EE_TYPE > AT24C16)      /* 24C16以上的型号, 分2个字节发送地址 */
    {
        iic_send_byte(0XA0);    /* 发送写命令, IIC规定最低位是0, 表示写入 */
        iic_wait_ack();         /* 每次发送完一个字节,都要等待ACK */
        iic_send_byte(addr >> 8);/* 发送高字节地址 */
    }
    else 
    {
        iic_send_byte(0XA0 + ((addr >> 8) << 1));   /* 发送器件 0XA0 + 高位a8/a9/a10地址,写数据 */
    }
    
    iic_wait_ack();             /* 每次发送完一个字节,都要等待ACK */
    iic_send_byte(addr % 256);  /* 发送低位地址 */
    iic_wait_ack();             /* 等待ACK, 此时地址发送完成了 */
    
    iic_start();                /* 重新发送起始信号 */ 
    iic_send_byte(0XA1);        /* 进入接收模式, IIC规定最低位是0, 表示读取 */
    iic_wait_ack();             /* 每次发送完一个字节,都要等待ACK */
    temp = iic_read_byte(0);    /* 接收一个字节数据 */
    iic_stop();                 /* 产生一个停止条件 */
    return temp;
}

/**
 * @brief       在ATK_24CXX指定地址写入一个数据
 * @param       addr: 写入数据的目的地址
 * @param       data: 要写入的数据
 * @retval      无
 */
void atk_24cxx_write_one_byte(uint16_t addr, uint8_t data)
{
    /* 原理说明见:at24cxx_read_one_byte函数, 本函数完全类似 */
    iic_start();                /* 发送起始信号 */

    if (EE_TYPE > AT24C16)      /* 24C16以上的型号, 分2个字节发送地址 */
    {
        iic_send_byte(0XA0);    /* 发送写命令, IIC规定最低位是0, 表示写入 */
        iic_wait_ack();         /* 每次发送完一个字节,都要等待ACK */
        iic_send_byte(addr >> 8);/* 发送高字节地址 */
    }
    else 
    {
        iic_send_byte(0XA0 + ((addr >> 8) << 1));   /* 发送器件 0XA0 + 高位a8/a9/a10地址,写数据 */
    }
    
    iic_wait_ack();             /* 每次发送完一个字节,都要等待ACK */
    iic_send_byte(addr % 256);  /* 发送低位地址 */
    iic_wait_ack();             /* 等待ACK, 此时地址发送完成了 */
    
    /* 因为写数据的时候,不需要进入接收模式了,所以这里不用重新发送起始信号了 */
    iic_send_byte(data);        /* 发送1字节 */
    iic_wait_ack();             /* 等待ACK */
    iic_stop();                 /* 产生一个停止条件 */
    delay_ms(10);               /* 注意: EEPROM 写入比较慢,必须等到10ms后再写下一个字节 */
}
 
/**
 * @brief       检查ATK_24CXX是否正常
 *   @note      检测原理: 在器件的末地址写如0X55, 然后再读取, 如果读取值为0X55
 *              则表示检测正常. 否则,则表示检测失败.
 *
 * @param       无
 * @retval      检测结果
 *              0: 检测成功
 *              1: 检测失败
 */
uint8_t atk_24cxx_check(void)
{
    uint8_t temp;
    uint16_t addr = EE_TYPE;
    temp = atk_24cxx_read_one_byte(addr); /* 避免每次开机都写AT24CXX */

    if (temp == 0X55)   /* 读取数据正常 */
    {
        return 0;
    }
    else    /* 排除第一次初始化的情况 */
    {
        atk_24cxx_write_one_byte(addr, 0X55); /* 先写入数据 */
        temp = atk_24cxx_read_one_byte(255);  /* 再读取数据 */

        if (temp == 0X55)return 0;
    }

    return 1;
}

/**
 * @brief       在ATK_24CXX里面的指定地址开始读出指定个数的数据
 * @param       addr    : 开始读出的地址 对24c02为0~255
 * @param       pbuf    : 数据数组首地址
 * @param       datalen : 要读出数据的个数
 * @retval      无
 */
void atk_24cxx_read(uint16_t addr, uint8_t *pbuf, uint16_t datalen)
{
    while (datalen--)
    {
        *pbuf++ = atk_24cxx_read_one_byte(addr++);
    }
}

/**
 * @brief       在ATK_24CXX里面的指定地址开始写入指定个数的数据
 * @param       addr    : 开始写入的地址 对24c02为0~255
 * @param       pbuf    : 数据数组首地址
 * @param       datalen : 要写入数据的个数
 * @retval      无
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
 * @brief 初始化 EEPROM 缓存：从 EEPROM 中恢复读写指针
 */
void atk_24cxx_eeprom_init(void)
{
    ee_write_index = atk_24cxx_read_one_byte(EE_START_ADDR);
    ee_read_index  = atk_24cxx_read_one_byte(EE_START_ADDR + 1);
}

/**
 * @brief 缓存当前传感器数据，循环覆盖最旧记录
 */
void buffer_current_reading(void)
{
    //printf("EEPROM存储数据,温度：%.1f,湿度：%.1f,空气质量：%.1f,火焰强度：%d\r\n",temperature, humidity, ppm, fire);
    Record r = { temperature, humidity, ppm, fire };
    uint16_t addr = EE_START_ADDR + 2 + ee_write_index * EE_RECORD_SIZE;
    atk_24cxx_write(addr, (uint8_t *)&r, EE_RECORD_SIZE);

    ee_write_index = (ee_write_index + 1) % EE_MAX_RECORDS;
    if (ee_write_index == ee_read_index) {
        ee_read_index = (ee_read_index + 1) % EE_MAX_RECORDS;
    }
    // 持久化指针
    atk_24cxx_write_one_byte(EE_START_ADDR, (uint8_t)ee_write_index);
    atk_24cxx_write_one_byte(EE_START_ADDR + 1, (uint8_t)ee_read_index);
}

/**
 * @brief 判断是否有待上传缓存数据
 */
bool has_buffered_data(void)
{
    return (ee_read_index != ee_write_index);
}

/**
 * @brief 读取下一条缓存记录并推进读指针
 * @param[out] r 目标 Record 指针
 */
void read_next_record(Record *r)
{
    uint16_t addr = EE_START_ADDR + 2 + ee_read_index * EE_RECORD_SIZE;
    atk_24cxx_read(addr, (uint8_t *)r, EE_RECORD_SIZE);
    ee_read_index = (ee_read_index + 1) % EE_MAX_RECORDS;
}



