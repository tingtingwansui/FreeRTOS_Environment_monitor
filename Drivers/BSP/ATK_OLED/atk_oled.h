/**
 ****************************************************************************************************
 * @file        atk_oled.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2024-11-01
 * @brief       OLED 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20241101
 * 第一次发布
 *
 ****************************************************************************************************
 */
 
#ifndef __OLED_H
#define __OLED_H

#include "stdlib.h" 
#include "./SYSTEM/sys/sys.h"


/* 命令/数据 定义 */
#define OLED_CMD        0       /* 写命令 */
#define OLED_DATA       1       /* 写数据 */

#define OLED_I2C_ADDR   (0x78)  /* OLED的设备地址 */
/******************************************************************************************/
    
static void atk_oled_wr_byte(uint8_t data, uint8_t cmd);    /* 写一个字节到OLED */
static uint32_t atk_oled_pow(uint8_t m, uint8_t n);         /* OLED求平方函数 */


void atk_oled_init(void);           /* OLED初始化 */
void atk_oled_clear(void);          /* OLED清屏 */
void atk_oled_display_on(void);     /* 开启OLED显示 */
void atk_oled_display_off(void);    /* 关闭OLED显示 */
void atk_oled_refresh_gram(void);   /* 更新显存到OLED */ 
void atk_oled_draw_point(uint8_t x, uint8_t y, uint8_t dot);    /* OLED画点 */
void atk_oled_fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot);        /* OLED区域填充 */
void atk_oled_show_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode); /* OLED显示字符 */
void atk_oled_show_num(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size);  /* OLED显示数字 */
void atk_oled_show_string(uint8_t x, uint8_t y, const char *p, uint8_t size);           /* OLED显示字符串 */
#endif




