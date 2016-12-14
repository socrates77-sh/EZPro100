
#ifndef CONFIG_H
#define CONFIG_H

#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"

typedef struct DEVICE_CONFIG
{
	u16  MCU_ID;
	u8   ProgramID;
	u16  RomFirAddr;
	u16  RomEndAddr;
	u16  OptionAddr;
	u8   OptionSize;
	u8   OptionProCnt;
	u8   BlankCode;

	u8   IRC_OPTION; //irc adjust loadvalue
	u16	 IRC_FrMax;
	u16   IRC_FrMin;
	u16   IRC_FrType;
	
	u16 ProTime;
	u16 ProVDD;
	u16 ProVPP;
	u16 IrcVDD;
	u16 IrcVPP;
	u16 VerifyVDD;
	u16 VerifyVPP;

	u8  IRC_Select; // 0 =nul ,1=irc,2 = read and write
	u16 IRC_VALUE_ADDR; // m202,m202b:0x0004,,m101,m111:0x0002
	u8  IRC_Shift_Value ; // 校准值偏移量
    u8  WriteEn; // 低字节= F 时 写 ROM，高字节=F时写OPTION 0。
	u8	MCU_Type; //0x01:OTP ,0x02:MTP,0X03:flash
	u8  MCU_Name[10];

}DEVICE_CONFIG;// *DEVICE_CONFIG;

#define  MCU_Type_OTP   0X01
#define  MCU_Type_MTP   0x02
#define  MCU_Type_Flash 0x03

extern DEVICE_CONFIG DeviceConfig_xx;

typedef struct CRC_CONFIG
{
  u16 check_addr;
  u16 check_vdd;
  u16 read_vdd;
  u16 time_out;
  u16 ok_num;
  u16 time_read;
  u16 time_vdd;
  u16 crc_value;
  u16 error_num;

}CRC_CONFIG;// *CRC_CONFIG;
extern CRC_CONFIG CrcConfig_xx;

typedef struct WRITE_CONFIG
{
        u16  RomBit;//如0xffff 表示rom是16位的，0x1fff表示rom是13位的
        u8   ProTime;            //=0 ；=1 100us；=2 300us
        u8   ReadProAddrAndData;//=0 MCU不支持读编程地址和编程数据；=1支持读编程地址和编程数据
        u8   Page;              //=0 不分page;=1有page 
        u32  Addr_Flash_OptionStart;     //option 存放在flash中的起始地址
        u8   SecurityOption;    //加密位在option?
        u8   SecurityBit;       //加密位在bit?
	u8   WriteCheckNeed;   // 烧写时是否需要进行普通读取校验:=0不需要普通校验；=1需要普通校验
	u8   ReadCnt;      // 需要读多少次
	u8   ReadOkCnt;    // 读成功多少次算烧写成功
	u8   ReWriteCnt;   // 烧写时普通读校验不读，尝试烧写总次数 
        u8   ForceWriteCnt;//即使烧写时普通校验过，也要强制烧写的总次数
	u8   AllCheckNeed; // 烧写完是否需要校验: =0不校验；=1普通校验；=2 Margin-1和Off Margin校验;=3 普通校验和Margin-1校验
	u16  OptionValue[16];
        u16  OptionAddr[16];

}WRITE_CONFIG;// 烧录芯片时所需的一些参数
extern WRITE_CONFIG WriteConfig_xx;

//extern DEVICE_CONFIG const DeviceConfig_M102R;
//extern DEVICE_CONFIG const DeviceConfig_M111;
//extern DEVICE_CONFIG const DeviceConfig_M224;
//extern DEVICE_CONFIG const DeviceConfig_M202;
//extern DEVICE_CONFIG const DeviceConfig_M201;
//extern DEVICE_CONFIG const DeviceConfig_T201;
//extern DEVICE_CONFIG const DeviceConfig_M101;
//extern DEVICE_CONFIG const DeviceConfig_M234;
//extern DEVICE_CONFIG const DeviceConfig_M202B;
//extern DEVICE_CONFIG const DeviceConfig_M238;
#endif