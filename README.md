# STM32自制badUSB

## 0x01	简介

​		近期拿出吃灰的stm32f407开发板，研究学习一下hid设备的开发，来实现一个低成本的badusb，本文使用开发板来进行测试，当然有条件的小伙伴还可以进行PCB打样，打印外壳来实现一个隐蔽性相对高的badusb。

​		BadUSB是一种伪装USB HID设备的攻击，hid设备是直接与人交互的设备，例如键盘、鼠标及游戏手柄等。不过HID设备并不一定要有人机接口，只要符合HID类别规范的设备都是HID设备。一般HID的攻击主要集中在鼠标键盘上，因为只要伪装成了用户交互设备，基本上就可以和用户的电脑进行交互，从而达到攻击的目的，而这一过程都是模拟人工操作，所以对于杀软来说就没办法进行查杀。应对这一攻击最有效的方法就是不要随意插入未知、不受信任的USB设备。

## 0x02	环境准备

- [x] stm32F407ZGT6（其它stm32芯片也行，不过要注意时钟，下文会说到）
- [x] stm32cubeMX
- [x] vscode
- [x] 交叉编译环境（本文使用Linux子系统+arm-gcc）
- [x] jlink调试器（其它下载方式也行）

## 0x03	初始化

​		本文采用HAL库开发，使用stm32cubeMX来生成一个基本工程项目文件

​		这里使用普遍的USB2.0的全速模式（FS），首先在Systen Core-RCC中配置为外部时钟

![image-20220504102702042](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20220504102702042.png)

​		在Connectivity-USB_OTG_FS中配置Mode为Device_Only

![image-20220504102837849](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20220504102837849.png)

​		在Middleware-USB_DEVICE中配置Class For FS IP 为HID设备，可在配置项修改vip、pid、描述字符串等。

![image-20220504103225523](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20220504103225523.png)

​		FS的最大速率在12Mbps，USB的系统时钟要传输速率的四倍，因此USB的系统时钟要配置为48Mhz，这里使用外部晶振通过倍频得到，小伙伴们可以参考下图的时钟树配置（注意自己芯片的外部晶振频率）。

![image-20220504101715768](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20220504101715768.png)

​		至此，基本的一个USB工程配置完毕，根据自己的环境生成相应的工程文件即可，这里稍微增大了堆栈的空间

![image-20220504103547397](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20220504103547397.png)

## 0x04	HID描述符配置

​		项目生成之后需要修改HID描述符，cubeMX默认生成的是鼠标设备，需要修改成键盘的描述符，报告描述符相当于HID设备的属性表。修改Middlewares\ST\STM32_USB_Device_Library\Class\HID\Src\usbd_hid.c中的HID_MOUSE_ReportDesc为键盘描述符，顺便修改数组大小常量HID_MOUSE_REPORT_DESC_SIZE为63。

```c
__ALIGN_BEGIN static uint8_t HID_MOUSE_ReportDesc[HID_MOUSE_REPORT_DESC_SIZE] __ALIGN_END =
{
  0x05, 0x01,// USAGE_PAGE (Generic Desktop)
  0x09, 0x06,// USAGE (Keyboard)
  0xa1, 0x01,// COLLECTION (Application)
  0x05, 0x07,// USAGE_PAGE (Keyboard)
  0x19, 0xe0,// USAGE_MINIMUM (Keyboard LeftControl)
  0x29, 0xe7,// USAGE_MAXIMUM (Keyboard Right GUI)
  0x15, 0x00,// LOGICAL_MINIMUM (0)
  0x25, 0x01,// LOGICAL_MAXIMUM (1)
  0x75, 0x01,// REPORT_SIZE (1)
  0x95, 0x08,// REPORT_COUNT (8)
  0x81, 0x02,// INPUT (Data,Var,Abs)
  0x95, 0x01,// REPORT_COUNT (1)
  0x75, 0x08,// REPORT_SIZE (8)
  0x81, 0x03,// INPUT (Cnst,Var,Abs)
  0x95, 0x05,// REPORT_COUNT (5)
  0x75, 0x01,// REPORT_SIZE (1)
  0x05, 0x08,// USAGE_PAGE (LEDs)
  0x19, 0x01,// USAGE_MINIMUM (Num Lock)
  0x29, 0x05,// USAGE_MAXIMUM (Kana)
  0x91, 0x02,// OUTPUT (Data,Var,Abs)
  0x95, 0x01,// REPORT_COUNT (1)
  0x75, 0x03,// REPORT_SIZE (3)
  0x91, 0x03,// OUTPUT (Cnst,Var,Abs)
  0x95, 0x06,// REPORT_COUNT (6)
  0x75, 0x08,// REPORT_SIZE (8)
  0x15, 0x00,// LOGICAL_MINIMUM (0)
  0x25, 0xFF,// LOGICAL_MAXIMUM (255)
  0x05, 0x07,// USAGE_PAGE (Keyboard)
  0x19, 0x00,// USAGE_MINIMUM (Reserved (no event indicated))
  0x29, 0x65,// USAGE_MAXIMUM (Keyboard Application)
  0x81, 0x00,// INPUT (Data,Ary,Abs)
  0xc0
};
```

​		修改USBD_HID_CfgDesc数组中的鼠标为键盘

![image-20220504105457608](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20220504105457608.png)

## 0x05	按键数据报文处理封装

​		上面的hid描述符决定了按键数据的报文格式，一个报文数据为8个字节，其具体意思可以描述为

> BYTE1 -- 特殊按键
>
> ​    |--bit0:  Left Control是否按下，按下为1  
>
> ​    |--bit1:  Left Shift  是否按下，按下为1  
>
> ​    |--bit2:  Left Alt   是否按下，按下为1  
>
> ​    |--bit3:  Left GUI（Windows键） 是否按下，按下为1  
>
> ​    |--bit4:  Right Control是否按下，按下为1  
>
> ​    |--bit5:  Right Shift 是否按下，按下为1  
>
> ​    |--bit6:  Right Alt  是否按下，按下为1  
>
> ​    |--bit7:  Right GUI  是否按下，按下为1 
>
> BYTE2 -- 暂不清楚，有的地方说是保留位
>
> BYTE3--BYTE8 -- 这六个为普通按键

​		第一个字节为四个功能键，只有六个字节为我们的普通按键，也就是说，我们一次可以操作六个普通按键，可能也就是所谓的六键无冲。具体六个按键的键值可以参考usb官方文档（https://usb.org/sites/default/files/hut1_3_0.pdf），发现其键值和基本的ASCII没啥联系，不可能写脚本的时候查表叭~~~所以这里就要自己封装一下，但好在其字母及数字部分是连续的，我们可以计算和ASCII的偏移来直接转码，不过特殊字符得手动处理一下喽，下面直接贴出代码，已实现常见需求，优化空间很大，小伙伴们可以自己优化一下呦~

```c
key_parse.h
#ifndef __KEY_PARSE_H
#define __KEY_PARSE_H
#define KEY_CONTROL 0x80>>3
#define KEY_SHIFT 0x80>>2
#define KEY_ALT 0X80>>1
#define KEY_WIN 0X80>>0
#define KEY_NULL 0x00          // NULL
#define KEY_ENTER 0x28         // ENTER
#define KEY_ESC 0x29           // ESC
#define KEY_BACKSPACE 0x2A     // BACKSPACE
#define KEY_TAB 0x2B           // TAB
#define KEY_F1 0x3A
#define KEY_F2 0x3B
#define KEY_F3 0x3C
#define KEY_F4 0x3D
#define KEY_F5 0x3E
#define KEY_F6 0x3F
#define KEY_F7 0x40
#define KEY_F8 0x41
#define KEY_F9 0x42
#define KEY_F10 0x43
#define KEY_F11 0x44
#define KEY_F12 0x45
#define KEY_PRT_SCR 0x46
#define KEY_SCOLL_LOCK 0x47
#define KEY_PAUSE 0x48
#define KEY_INS 0x49
#define KEY_HOME 0x4A
#define KEY_PAGEUP 0x4B
#define KEY_DEL 0x4C
#define KEY_END 0x4D
#define KEY_PAGEDOWN 0x4E
#define KEY_RIGHT_ARROW 0x4F
#define KEY_LEFT_ARROW 0x50
#define KEY_DOWN_ARROW 0x51
#define KEY_UP_ARROW 0x52
#define KEY_DELAY 25//HID发送延时
static unsigned char key_data[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//hid发送缓冲数据
unsigned char ascii_to_key(unsigned char ascii);//转字母数字到一级字符，非一级字符返回0到parse_key处理
unsigned char parse_key(unsigned char key);//转二级字符（需按shift的字符）
void key_print(unsigned char *string);//键盘输出字符串
void key_press(unsigned char key);//按下功能键
void key_unpress(void);//弹起功能键
void key_unpress_all(void);//弹起所有键
void pressFunction(unsigned char key);//按下特殊键
#endif
```

```c
key_parse.c
#include "usbd_hid.h"
#include "usb_device.h"
#include "key_parse.h"
extern USBD_HandleTypeDef hUsbDeviceFS;
unsigned char ascii_to_key(unsigned char ascii){
    if(32<=ascii&&ascii<=126){
        if(0x41<=ascii&&ascii<=0x5a){//大写字母
            return ascii-0x3d;
        }
        if(0x61<=ascii&&ascii<=0x7a){//小写字母
            return ascii-0x5d;
        }
        if(0x30<=ascii&&ascii<=0x39){//数字
            if(ascii==0x30) return 0x27;
            return ascii-0x13;
        }
        switch (ascii)
        {
            case '-':return 0x2d;
            case '=':return 0x2e;
            case '[':return 0x2f;
            case ']':return 0x30;
            case ';':return 0x33;
            case 0x27:return 0x34;
            case 0x5c:return 0x31;
            case ',':return 0x36;
            case '.':return 0x37;
            case '/':return 0x38;
            case ' ':return KEY_SPACE;
            default:return 0;
        }
    }else{
        return 0;
    }
    return 0;
}
unsigned char parse_key(unsigned char key){
    //unsigned char temp=ascii_to_key(key);
    switch (key)
    {
        case '!':return ascii_to_key('1');break;
        case '@':return ascii_to_key('2');break;
        case '#':return ascii_to_key('3');break;
        case '$':return ascii_to_key('4');break;
        case '%':return ascii_to_key('5');break;
        case '^':return ascii_to_key('6');break;
        case '&':return ascii_to_key('7');break;
        case '*':return ascii_to_key('8');break;
        case '(':return ascii_to_key('9');break;
        case ')':return ascii_to_key('0');break;
        case '_':return ascii_to_key('-');break;
        case '+':return ascii_to_key('=');break;
        case '{':return ascii_to_key('[');break;
        case '}':return ascii_to_key(']');break;
        case ':':return ascii_to_key(';');break;
        case '"':return ascii_to_key(0x27);break;
        case '|':return ascii_to_key(0x5c);break;
        case '<':return ascii_to_key(',');break;
        case '>':return ascii_to_key('.');break;
        case '?':return ascii_to_key('/');break;
        default:return key;
    }
    return key;
}
void key_print(unsigned char *string){
/*
输出字符
*/
    unsigned int i,j,nextKey,temp;
    i=0;
    j=0;
    while(string[i]!='\0'){ 
        temp=ascii_to_key(string[i]);
        nextKey=ascii_to_key(string[i+1]);
        if(temp){
            key_data[2+j]=temp;
            j++;
            if(j==6){
                j=0;
                USBD_HID_SendReport(&hUsbDeviceFS,key_data,8);
                HAL_Delay(KEY_DELAY);
                key_unpress_all();
                HAL_Delay(KEY_DELAY);
            }else if(!nextKey||parse_key(nextKey)==temp){
                j=0;
                USBD_HID_SendReport(&hUsbDeviceFS,key_data,8);
                HAL_Delay(KEY_DELAY);
                key_unpress_all();
                HAL_Delay(KEY_DELAY);
            }
        }else{
            temp=parse_key(string[i]);
            key_data[2+0]=temp;
            j++;
            if(j==6){
                j=0;
                key_data[0]=KEY_SHIFT;
                USBD_HID_SendReport(&hUsbDeviceFS,key_data,8);
                HAL_Delay(KEY_DELAY);
                key_unpress_all();
                HAL_Delay(KEY_DELAY);
            }else if(nextKey||nextKey==temp){
                j=0;
                key_data[0]=KEY_SHIFT;
                USBD_HID_SendReport(&hUsbDeviceFS,key_data,8);
                HAL_Delay(KEY_DELAY);
                key_unpress_all();
                HAL_Delay(KEY_DELAY);
            }
        }
        i++;
        //if(string[i]=='\0') return;
    }
}
void key_press(unsigned char key){
/*
按下功能键盘
*/
    key_data[0]=key;
    USBD_HID_SendReport(&hUsbDeviceFS,key_data,8);
    HAL_Delay(KEY_DELAY);
}
void key_unpress(void){
/*
弹起功能键盘
*/
    key_data[0]=0x00;
    USBD_HID_SendReport(&hUsbDeviceFS,key_data,8);
}
void key_unpress_all(void){
/*
弹起所有键
*/
    for(unsigned char i=0;i<8;i++){
        key_data[i]=0x00;
    }
    USBD_HID_SendReport(&hUsbDeviceFS,key_data,8);
}

void pressFunction(unsigned char key){
    key_data[2]=key;
    USBD_HID_SendReport(&hUsbDeviceFS,key_data,8);
    HAL_Delay(KEY_DELAY);
    key_unpress_all();
    HAL_Delay(KEY_DELAY);
}
```

