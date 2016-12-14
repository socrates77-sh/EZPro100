
#ifndef global_h
#define golbal_h

#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"
#define VIRTUAL_COM_PORT_DATA_SIZE              64
#define IIC_DATA_BUFF_SIZE			32
#define HIRC_TEMP                               0x00      //MC9033内部高频振荡器温度校准位 OPBIT2 BIT[7:3]
#define HIRC_VOLTAGE                            0x00       //MC9033HIRC电压选择位 OPBIT3 BIT[4]

extern u16 IRC_VALUE;
extern u16 TEST_IRC[100];//测试用
extern u16 TEST_COUNT;//测试用
extern u16 OTP_ADDR;
extern u32 OKcounter;
extern u16 NGcounter;
extern u16 USB_RxDataCS;
extern u8  WR_Command; 
extern u8  USB_RxCommand;
extern u8	IRC_MODEL;
extern u8  OTP_ReadData;
extern u8  OPTION_FRT;
extern u8   OPTION_FRT2;
extern u8  OPTION_TEMPADJ;
extern u16  OPTION_TEMP;//
extern u16 OSCCAL;//频率校准值
extern u8  TADJ;//温度校准值
extern u8  ERORR_VALUE; //Return error number
extern u16 adc_value[3];
extern u8 WriteFlag;//=1 mc32t8132在线烧录；=0 mc32t8132裸片烧录

extern u8 USB_Rx_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
extern u8 USB_Tx_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
extern u8 USB_Rx_Flag;
extern u8 StateFlag;
extern u8 ModelIn_result;
extern u8 iic_data[IIC_DATA_BUFF_SIZE];
extern u8 OP_STATE;

#define LEDOK_Off GPIO_SetBits(GPIOC,GPIO_Pin_10)
#define LEDOK_On  GPIO_ResetBits(GPIOC,GPIO_Pin_10)

#define LEDNG_Off GPIO_SetBits(GPIOC,GPIO_Pin_11)
#define LEDNG_On  GPIO_ResetBits(GPIOC,GPIO_Pin_11)

#define LEDBUSY_Off GPIO_SetBits(GPIOC,GPIO_Pin_12)
#define LEDBUSY_On  GPIO_ResetBits(GPIOC,GPIO_Pin_12)

//关断Q5后，如果Q14导通则D8导通，VPP差不多等于VDD,9033进入模式的时序需要这样，6060进模式不需要这样的VPP时序
#define CLN_VDD2GND_On GPIO_SetBits(GPIOC,GPIO_Pin_13)
#define CLN_VDD2GND_Off  GPIO_ResetBits(GPIOC,GPIO_Pin_13)

#define USB_REG_On   GPIO_SetBits(GPIOB,GPIO_Pin_1)
#define USB_REG_Off  GPIO_ResetBits(GPIOB,GPIO_Pin_1)

#define FlagRead  GPIOD,GPIO_Pin_7
#define FlagProtect  GPIOD,GPIO_Pin_6
#define FlagBlank  GPIOD,GPIO_Pin_5
#define FlagVerify GPIOD,GPIO_Pin_4
#define FlagWrite  GPIOD,GPIO_Pin_3

#define KEY  GPIOD,GPIO_Pin_10


extern void Writer_Test(); //test.c

extern void RCC_Configuration(void);
extern void GPIO_Configuration(void);
extern void Freq(u8 Numb);
extern void Delay_1us(u16 cnt );
extern void Delay_10us(u16 cnt );
extern void Delay_100us(u16 cnt );
extern void Delay_1ms(u16 cnt );
extern void DeviceConfig();

#endif