#include "stm32f10x.h"

#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"

#include "stm32f10x_tim.h"
#include "global.h"
#include "spi.h"
#include "power.h"
#include "OTPRW.h"
#include "IRC.h"
#include "M201IRC.h"
#include "MC224BPro.h"
#include "Config.h"



DEVICE_CONFIG DeviceConfig_xx=
{
	0x0353, //MCU_ID
	0x00,	//ProgramID
	0x1800,	//RomFirAddr
	0x1fff,  //RomEndAddr
	0x0000,	//OptionAddr
	0x02,	//OptionSize
	0x02,	//OptionProCnt  //现在基本不用
	0xff,	//BlankCode     //现在基本不用

	0x00,	//IRC_OPTION    //现在基本不用，以前只有一个OPTION字节时，上位机直接发配置信息给烧写器
	0x65,  //IRC_FrMax
	0x2e,   //ICR_FrMin
	0x40,	//IRC_FrType

	2000,	//ProTime       //如9033开发手册5.4节OTP编程时序图，有个大于100us，现在一般设置4
	0x0605,	//ProVDD        //现在基本不用
	150,	//ProVPP        
	25,	    //IrcVDD
	150,  //IrcVPP          //现在基本不用
	0x0300,	//VerifyVDD     //现在基本不用
	0x0b00, //VerifyVPP     //现在基本不用

	0x00,   //IRC_Selsct    //IRC_Selsct 00=NULL , 01=IRC ,02 = Read and Write
	0x0002, //IRC_VALUE_ADDR 现在不用，以前是个地址数据，该地址存放IRC校准得到的数据
	0x06,   //IRC_Shift_Value 现在基本不用 
        0xff, // u8  WriteEn; 0xf0--Option, 0x0f--ROM
	0x01,	//mcu type: 0x01:OTP ,0x02 :MTP ,0X03 :flash
	//7D27 4D433130        419     db 'MC10P02R';
	//7D2B 50303252
	{0x4d,	//mcu Name
	0x43,	//mcu Name
	0x31,	//mcu Name
	0x30,	//mcu Name
	0x50,	//mcu Name
	0x30,	//mcu Name
	0x32,	//mcu Name
	0x52,	//mcu Name
	0x20,	//mcu Name
	0x20}	//mcu Name
};

CRC_CONFIG CrcConfig_xx;

WRITE_CONFIG WriteConfig_xx=
{
        0xffff,
        0,
        1,
        0,
        0,
        0,
        15,
	0,   
	1,   
	1,   
	3, 
        1,
	2,   
	{0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff},//默认Option值
	{0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff}//默认Option值
};
/*
DEVICE_CONFIG const DeviceConfig_M102R=
{
	0x0353, //MCU_ID
	0x01,	//ProgramID
	0x1800,	//RomFirAddr
	0x1fff,  //RomEndAddr
	0x0000,	//OptionAddr
	0x02,	//OptionSize
	0x02,	//OptionProCnt
	0xff,	//BlankCode

	0x00,	//IRC_OPTION
	4040,  //IRC_FrMax
	3960,   //ICR_FrMin
	4000,	//IRC_FrType

	2000,	//ProTime
	0x0605,	//ProVDD
	0x0b00,	//ProVPP
	0x0300,	//IrcVDD
	0x0b00,  //IrcVPP
	0x0300,	//VerifyVDD
	0x0b00, //VerifyVPP

	0x00,   //IRC_Selsct
	0x0002, //IRC_VALUE_ADDR
	0x00,   //IRC_Shift_Value
    0xff,// u8  WriteEn;
	0x01,	//mcu type: 0x01:OTP ,0x02 :MTP ,0X03 :flash
	//7D27 4D433130        419     db 'MC10P02R';
	//7D2B 50303252
	0x4d,	//mcu Name
	0x43,	//mcu Name
	0x31,	//mcu Name
	0x30,	//mcu Name
	0x50,	//mcu Name
	0x30,	//mcu Name
	0x32,	//mcu Name
	0x52,	//mcu Name
	0x20,	//mcu Name
	0x20,	//mcu Name
};

DEVICE_CONFIG const DeviceConfig_M111=
{
	0x0111, //MCU_ID
	0x02,	//ProgramID
	0x1c00,	//RomFirAddr
	0x1fff,  //RomEndAddr
	0x0000,	//OptionAddr
	0x02,	//OptionSize
	0x02,	//OptionProCnt
	0xff,	//BlankCode

	0xff,	//IRC_OPTION
	4040,  //IRC_FrMax
	3960,   //ICR_FrMin
	4000,	//IRC_FrType

	2000,	//ProTime
	0x0605,	//ProVDD
	0x0b00,	//ProVPP
	0x0300,	//IrcVDD
	0x0b00,  //IrcVPP
	0x0300,	//VerifyVDD
	0x0b00, //VerifyVPP

	0x01,   //IRC_Selsct
	0x0002, //IRC_VALUE_ADDR
	0x00,   //IRC_Shift_Value
    0xff,// u8  WriteEn;
	0x01,	//mcu type: 0x01:OTP ,0x02 :MTP ,0X03 :flash
	//7D27 4D433130        419     db 'MC10P11';
	//7D2B 503131
	0x4d,	//mcu Name
	0x43,	//mcu Name
	0x31,	//mcu Name
	0x30,	//mcu Name
	0x50,	//mcu Name
	0x31,	//mcu Name
	0x31,	//mcu Name
	0x20,	//mcu Name
	0x20,	//mcu Name
	0x20,	//mcu Name
};

DEVICE_CONFIG const DeviceConfig_M224=
{
	0x0224, //MCU_ID
	0x00,	//ProgramID
	0x1000,	//RomFirAddr
	0x1fff,  //RomEndAddr
	0x0000,	//OptionAddr
	0x01,	//OptionSize,0x01:not IRC,0x02 IRC
	0x01,	//OptionProCnt
	0xff,	//BlankCode

	0x00,	//IRC_OPTION
	0x0065,  //IRC_FrMax
	0x002e,   //ICR_FrMin
	0x0040,	//IRC_FrType

	2000,	//ProTime
	0x0605,	//ProVDD
	0x0b00,	//ProVPP
	0x0500,	//IrcVDD
	0x0b00,  //IrcVPP
	0x0500,	//VerifyVDD
	0x0b00, //VerifyVPP

	0x00,   //IRC_Selsct
	0x0002, //IRC_VALUE_ADDR
	0x00,   //IRC_Shift_Value
    0xff,// u8  WriteEn;
	0x01,	//mcu type: 0x01:OTP ,0x02 :MTP ,0X03 :flash
	//7D27 4D433230        419     db 'MC20P24';
	//7D2B 503234
	0x4d,	//mcu Name
	0x43,	//mcu Name
	0x32,	//mcu Name
	0x30,	//mcu Name
	0x50,	//mcu Name
	0x32,	//mcu Name
	0x34,	//mcu Name
	0x20,	//mcu Name
	0x20,	//mcu Name
	0x20,	//mcu Name
};
DEVICE_CONFIG const DeviceConfig_M224B=
{
	0x0224, //MCU_ID
	0x00,	//ProgramID
	0x1000,	//RomFirAddr
	0x1fff,  //RomEndAddr
	0x0000,	//OptionAddr
	0x02,	//OptionSize,0x01:not IRC,0x02 IRC
	0x02,	//OptionProCnt
	0xff,	//BlankCode

	0xc7,	//IRC_OPTION
	253,  //IRC_FrMax
	247,   //ICR_FrMin
	250,	//IRC_FrType

	2000,	//ProTime
	0x0605,	//ProVDD
	150,	//ProVPP
	51,	//IrcVDD
	0x0b00,  //IrcVPP
	0x0500,	//VerifyVDD
	0x0b00, //VerifyVPP

	0x00,   //IRC_Selsct
	0x0001, //IRC_VALUE_ADDR
	0x00,   //IRC_Shift_Value
    0x00,// u8  WriteEn;
	0x01,	//mcu type: 0x01:OTP ,0x02 :MTP ,0X03 :flash
	//7D27 4D433230        419     db 'MC20P24B';
	//7D2B 50323442
	0x4d,	//mcu Name
	0x43,	//mcu Name
	0x32,	//mcu Name
	0x30,	//mcu Name
	0x50,	//mcu Name
	0x32,	//mcu Name
	0x34,	//mcu Name
	0x42,	//mcu Name
	0x20,	//mcu Name
	0x20,	//mcu Name
};

DEVICE_CONFIG const DeviceConfig_M202=
{
	0x0202, //MCU_ID
	0x01,	//ProgramID
	0x1800,	//RomFirAddr
	0x1fff,  //RomEndAddr
	0x0000,	//OptionAddr
	0x02,	//OptionSize
	0x02,	//OptionProCnt
	0xff,	//BlankCode
	
	0x00,	//IRC_OPTION
	4040,  //IRC_FrMax
	3960,   //ICR_FrMin
	4000,	//IRC_FrType
	
	2000,	//ProTime
	0x0605,	//ProVDD
	0x0b00,	//ProVPP
	0x0300,	//IrcVDD
	0x0b00,  //IrcVPP
	0x0300,	//VerifyVDD
	0x0b00, //VerifyVPP

	0x00,   //IRC_Selsct
	0x0002, //IRC_VALUE_ADDR
	0x00,   //IRC_Shift_Value
    0xff,// u8  WriteEn;
	0x01,	//mcu type: 0x01:OTP ,0x02 :MTP ,0X03 :flash
	//7D27 4D433230        419     db 'MC20P02';
	//7D2B 50303220
	0x4d,	//mcu Name
	0x43,	//mcu Name
	0x32,	//mcu Name
	0x30,	//mcu Name
	0x50,	//mcu Name
	0x30,	//mcu Name
	0x32,	//mcu Name
	0x20,	//mcu Name
	0x20,	//mcu Name
	0x20,	//mcu Name
};
DEVICE_CONFIG const DeviceConfig_M201=
{
	0x0201, //MCU_ID
	0x00,	//ProgramID
	0x1c00,	//RomFirAddr
	0x1fff,  //RomEndAddr
	0x0000,	//OptionAddr
	0x02,	//OptionSize
	0x02,	//OptionProCnt
	0xff,	//BlankCode

	0x32,	//IRC_OPTION
	253,  //IRC_FrMax
	247,   //ICR_FrMin
	250,	//IRC_FrType

	2000,	//ProTime
	0x0605,	//ProVDD
	0x0b00,	//ProVPP
	0x0300,	//IrcVDD
	0x0b00,  //IrcVPP
	0x0300,	//VerifyVDD
	0x0b00, //VerifyVPP

	0x02,   //IRC_Selsct
	0x0004, //IRC_VALUE_ADDR
	0x00,   //IRC_Shift_Value
    0xFF,// u8  WriteEn;
	0x01,	//mcu type: 0x01:OTP ,0x02 :MTP ,0X03 :flash
	//7D27 4D433130        419     db 'MC20P01';
	//7D2B 50303252
	0x4d,	//mcu Name
	0x43,	//mcu Name
	0x32,	//mcu Name
	0x30,	//mcu Name
	0x50,	//mcu Name
	0x30,	//mcu Name
	0x31,	//mcu Name
	0x20,	//mcu Name
	0x20,	//mcu Name
	0x20,	//mcu Name
};
DEVICE_CONFIG const DeviceConfig_T201=
{
	0x0201, //MCU_ID
	0x03,	//ProgramID
	0x2000,	//RomFirAddr
	0x3fff,  //RomEndAddr
	0x0000,	//OptionAddr
	0x02,	//OptionSize
	0x02,	//OptionProCnt
	0xff,	//BlankCode

	0x32,	//IRC_OPTION
	253,  //IRC_FrMax
	247,   //ICR_FrMin
	250,	//IRC_FrType

	55000,	//ProTime
	0x0605,	//ProVDD
	110,	//ProVPP
	51,		//IrcVDD
	0x0b00,  //IrcVPP
	0x0300,	//VerifyVDD
	0x0b00, //VerifyVPP

	0x00,   //IRC_Selsct
	0x0004, //IRC_VALUE_ADDR
	0x00,   //IRC_Shift_Value
	0xFF,// u8  WriteEn;
	0x02,	//mcu type: 0x01:OTP ,0x02 :MTP ,0X03 :flash
	//7D27 4D433230        419     db 'MC20T01   ';
	//7D2B 54303120
	0x4d,	//mcu Name
	0x43,	//mcu Name
	0x32,	//mcu Name
	0x30,	//mcu Name
	0x54,	//mcu Name
	0x30,	//mcu Name
	0x31,	//mcu Name
	0x20,	//mcu Name
	0x20,	//mcu Name
	0x20,	//mcu Name
};

DEVICE_CONFIG const DeviceConfig_M101=
{
	0x0101, //MCU_ID1
	0x00,	//ProgramID
	0x1C00,	//RomFirAddr
	0x0000,//0x1fff, //RomEndAddr
	0x0000,	//OptionAddr
	0x02,	//OptionSize
	0x02,	//OptionProCnt
	0xff,	//BlankCode

	0x08,	//IRC_OPTION
	4030,  //IRC_FrMax
	3990,   //ICR_FrMin
	4010,	//IRC_FrType

	2000,	//ProTime
	0x0605,	//ProVDD
	150,	//ProVPP
	25,	//IrcVDD
	0x0b00,  //IrcVPP
	0x0300,	//VerifyVDD
	0x0b00, //VerifyVPP

	0x00,   //IRC_Selsct // 0 =nul ,1=irc,2 = read and write
	0x0002, //IRC_VALUE_ADDR
	0x06,   //IRC_Shift_Value
        0xff,// u8  WriteEn;
	0x01,	//mcu type: 0x01:OTP ,0x02 :MTP ,0X03 :flash
	//7D27 4D433130        419     db 'MC10P01';
	//7D2B 50303252
	0x4d,	//mcu Name
	0x43,	//mcu Name
	0x31,	//mcu Name
	0x30,	//mcu Name
	0x50,	//mcu Name
	0x30,	//mcu Name
	0x31,	//mcu Name
	0x20,	//mcu Name
	0x20,	//mcu Name
	0x20,	//mcu Name
};

DEVICE_CONFIG const DeviceConfig_M234=
{
	0x0234, //MCU_ID1
	0x00,	//ProgramID
	0x1000,	//RomFirAddr
	0x1fff, //RomEndAddr
	0x0000,	//OptionAddr
	0x02,	//OptionSize
	0x02,	//OptionProCnt
	0xff,	//BlankCode

	0x08,	//IRC_OPTION
	253,  //IRC_FrMax
	247,   //ICR_FrMin
	250,	//IRC_FrType

	2000,	//ProTime
	0x0605,	//ProVDD
	0x0b00,	//ProVPP
	0x0300,	//IrcVDD
	0x0b00,  //IrcVPP
	0x0300,	//VerifyVDD
	0x0b00, //VerifyVPP

	0x01,   //IRC_Selsct
	0x0002, //IRC_VALUE_ADDR
	0x00,   //IRC_Shift_Value
    0xff,// u8  WriteEn;
	0x01,	//mcu type: 0x01:OTP ,0x02 :MTP ,0X03 :flash
	//7D27 4D433130        419     db 'MC20P34;
	//7D2B 50303252
	0x4d,	//mcu Name
	0x43,	//mcu Name
	0x32,	//mcu Name
	0x30,	//mcu Name
	0x50,	//mcu Name
	0x33,	//mcu Name
	0x34,	//mcu Name
	0x20,	//mcu Name
	0x20,	//mcu Name
	0x20,	//mcu Name
};

DEVICE_CONFIG const DeviceConfig_M202B=
{
	0x0234, //MCU_ID1
	0x00,	//ProgramID
	0x1000,	//RomFirAddr
	0x1fff, //RomEndAddr
	0x0000,	//OptionAddr
	0x02,	//OptionSize
	0x02,	//OptionProCnt
	0xff,	//BlankCode

	0x08,	//IRC_OPTION
	253,  //IRC_FrMax
	247,   //ICR_FrMin
	250,	//IRC_FrType

	2000,	//ProTime
	0x0605,	//ProVDD
	0x0b00,	//ProVPP
	0x0300,	//IrcVDD
	0x0b00,  //IrcVPP
	0x0300,	//VerifyVDD
	0x0b00, //VerifyVPP

	0x01,   //IRC_Selsct
	0x0002, //IRC_VALUE_ADDR
	0x00,   //IRC_Shift_Value
    0xff,// u8  WriteEn;
	0x01,	//mcu type: 0x01:OTP ,0x02 :MTP ,0X03 :flash
	//7D27 4D433130        419     db 'MC20P02B';
	//7D2B 50303252
	//MCU_Name[10]={
	0x4d,	//mcu Name
	0x43,	//mcu Name
	0x32,	//mcu Name
	0x30,	//mcu Name
	0x50,	//mcu Name
	0x30,	//mcu Name
	0x32,	//mcu Name
	0x42,	//mcu Name
	0x20,	//mcu Name
	0x20,	//mcu Name
};

DEVICE_CONFIG const DeviceConfig_M238=
{
	0x0238, //MCU_ID1
	0x03,	//ProgramID
	0x2000,	//RomFirAddr
	0x3fff, //RomEndAddr
	0x0000,	//OptionAddr
	0x02,	//OptionSize
	0x02,	//OptionProCnt
	0xff,	//BlankCode

	0xff,	//IRC_OPTION
	253,  //IRC_FrMax
	247,   //ICR_FrMin
	250,	//IRC_FrType

	2000,	//ProTime
	51,	    //ProVDD
	150,	//ProVPP
	51,	//IrcVDD
	0x0b00,  //IrcVPP
	0x0300,	//VerifyVDD
	0x0b00, //VerifyVPP

	0x01,   //IRC_Selsct // 0 =nul ,1=irc,2 = read and write
	0x0002, //IRC_VALUE_ADDR
	0x00,   //IRC_Shift_Value
	0x00,// u8  WriteEn;
	0x01,	//mcu type: 0x01:OTP ,0x02 :MTP ,0X03 :flash
	//7D27 4D433130        419     db 'MC20P38';
	//7D2B 50303252
	//MCU_Name[10]={
	0x4d,	//mcu Name
	0x43,	//mcu Name
	0x32,	//mcu Name
	0x30,	//mcu Name
	0x50,	//mcu Name
	0x33,	//mcu Name
	0x38,	//mcu Name
	0x20,	//mcu Name
	0x20,	//mcu Name
	0x20,	//mcu Name
};*/