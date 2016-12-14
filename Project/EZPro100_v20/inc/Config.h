
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
	u8  IRC_Shift_Value ; // У׼ֵƫ����
    u8  WriteEn; // ���ֽ�= F ʱ д ROM�����ֽ�=FʱдOPTION 0��
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
        u16  RomBit;//��0xffff ��ʾrom��16λ�ģ�0x1fff��ʾrom��13λ��
        u8   ProTime;            //=0 ��=1 100us��=2 300us
        u8   ReadProAddrAndData;//=0 MCU��֧�ֶ���̵�ַ�ͱ�����ݣ�=1֧�ֶ���̵�ַ�ͱ������
        u8   Page;              //=0 ����page;=1��page 
        u32  Addr_Flash_OptionStart;     //option �����flash�е���ʼ��ַ
        u8   SecurityOption;    //����λ��option?
        u8   SecurityBit;       //����λ��bit?
	u8   WriteCheckNeed;   // ��дʱ�Ƿ���Ҫ������ͨ��ȡУ��:=0����Ҫ��ͨУ�飻=1��Ҫ��ͨУ��
	u8   ReadCnt;      // ��Ҫ�����ٴ�
	u8   ReadOkCnt;    // ���ɹ����ٴ�����д�ɹ�
	u8   ReWriteCnt;   // ��дʱ��ͨ��У�鲻����������д�ܴ��� 
        u8   ForceWriteCnt;//��ʹ��дʱ��ͨУ�����ҲҪǿ����д���ܴ���
	u8   AllCheckNeed; // ��д���Ƿ���ҪУ��: =0��У�飻=1��ͨУ�飻=2 Margin-1��Off MarginУ��;=3 ��ͨУ���Margin-1У��
	u16  OptionValue[16];
        u16  OptionAddr[16];

}WRITE_CONFIG;// ��¼оƬʱ�����һЩ����
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