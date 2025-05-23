一、项目概述
本项目是基于STM32微控制器与FreeRTOS实时操作系统的智能环境监测系统，集成多传感器融合技术、物联网通信和云端协同控制，实现对温度、湿度、空气质量、火焰强度等环境参数的实时监测与智能调控。
通过ESP8266模块实现数据上云，支持MQTT协议与云平台交互，具备断网数据缓存、异常报警功能。


二、硬件组成
1.核心控制器
STM32F103RET6（主频72MHz）
FreeRTOS多任务调度

2.传感器模块
ATK-AHT20：高精度温湿度传感器（量程：-40~85℃±0.3℃，0~100%RH±2%）
ATK-AIR：空气质量传感器（检测PM2.5、VOCs等，量程0-2000ppm）
ATK-FIRE：火焰传感器（检测火焰强度，量程0-4095）
ADC模块：多通道模拟信号采集

3.执行设备
ATK-FAN：PWM调速风扇（最大转速30级）
ATK-BUZZER：蜂鸣器报警模块

4.通信与显示
ATK-MB026（ESP8266）：WiFi模块（支持TCP/IP、MQTT协议）
0.96寸OLED：实时数据显示（128x64分辨率）
ATK-24CXX：EEPROM数据存储（断网缓存）


三、软件架构  
1.FreeRTOS任务设计
数据采集任务：周期性读取传感器数据（优先级5）
OLED显示任务：动态刷新环境参数（优先级2）
MQTT上传任务：数据分片上传至云平台（优先级4）
事件处理任务：高温自动启停风扇/火焰报警（优先级6）
EEPROM任务：断网时缓存数据至本地（优先级3）
2.关键算法
多源数据加权融合（温湿度补偿校准）
双重阈值报警机制（如温度>30℃启风扇，PPM>1000触发蜂鸣器）
数据分片上传策略（减少网络负载）


四、功能特点
1.实时监测
OLED显示温度、湿度、PM2.5浓度、火焰强度及综合环境状态（如"Humid"/"Poor air"）
串口调试输出实时数据（支持JSON格式）

2.智能控制
温度>30℃自动启动风扇
火焰强度>90%触发蜂鸣器报警
支持AT指令远程调控设备

3.云端交互
连接OneNET云平台（MQTT协议）
支持订阅/发布主题（如$sys/.../thing/property/post）
断网时数据本地缓存（EEPROM存储），网络恢复后自动续传

4.低功耗设计
动态任务休眠机制（非采集时段进入低功耗模式）
传感器间歇性工作（采集周期1s）


五、使用说明
1.硬件部署
将火焰传感器部署在易燃区域（如厨房）
空气质量传感器远离通风口以获取稳定数据
ESP8266天线需远离金属屏蔽物

2.参数配置
修改main.c中WiFi信息：
#define DEMO_WIFI_SSID "your_SSID"  
#define DEMO_WIFI_PWD "your_PASSWORD"  
调整云平台MQTT连接参数（服务器地址、客户端ID等）
密码要求特殊，需要使用token生成密码(产品ID，设备密钥，时间戳)

3.编译与烧录
使用Keil uVision5（需安装STM32F1xx_DFP库）
通过ST-Link下载程序，波特率115200

4.操作流程
上电初始化 → 传感器预热（20秒） → 自动连接WiFi → 数据上传云端