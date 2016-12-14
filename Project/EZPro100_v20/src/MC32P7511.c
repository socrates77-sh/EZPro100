#include "stm32f10x.h"
#include "global.h"
#include "spi.h"
#include "OTPRW.h"
#include "power.h"
#include "IRC.h"
#include "M201IRC.h"
#include "MC224BPro.h"
#include "Config.h"
#include "ADC.h"
#include "MC301Pro.h"
#include "erorrNum.h"
#include "MC32P21Pro.h"
#include "MC32P7511.h"
#include "MC9033.h"

uint8_t data[]= {
    0x28,0x10,
    };
uint16_t crc;
uint8_t i, chChar;
uint16_t wCRC = 0xFFFF;
//int main(void)
//{
//    uint16_t crc;
//    uint16_t ucrc;
//
// 	crc = CRC16(data,sizeof(data));
//
//    ucrc = uCRC16(0xf06e);
//
//    return 1;
//
//}

uint16_t CRC16(uint8_t* pchMsg, uint16_t wDataLen)
{
        uint8_t i, chChar;
        uint16_t wCRC = 0xFFFF;

        while (wDataLen--)
        {
                chChar = *pchMsg++;

                wCRC ^= (((uint16_t) chChar) << 8);

                for (i = 0; i < 8; i++)
                {
                        if (wCRC & 0x8000)
                                wCRC = (wCRC << 1) ^ CRC_16_POLYNOMIALS;
                        else
                                wCRC <<= 1;
                }
        }

        return wCRC;
}
u16 CountCRC(u8 data1,u8 data2,u16 crc)
{
  u8 i;
  crc ^= (((uint16_t) data1) << 8);

  for (i = 0; i < 8; i++)
  {
        if (crc & 0x8000)
                crc = (crc << 1) ^ CRC_16_POLYNOMIALS;
        else
                crc <<= 1;
  }
  crc ^= (((uint16_t) data2) << 8);

  for (i = 0; i < 8; i++)
  {
        if (crc & 0x8000)
                crc = (crc << 1) ^ CRC_16_POLYNOMIALS;
        else
                crc <<= 1;
  }      
  return crc;
}
/*******************************************************************************
* Function Name  : GetCheckSum
* Description    : 取OTP校验值
* Input          : check_num : 校验OTP个数（从ROM区0x0地址开始计算CRC校验值）
* Output         : 
* Return         : a : OTP校验值
*******************************************************************************/
u16 GetCheckSum(u16 check_num)
{
  u16 a;
  SetOption(0xFFF0,0xff8f);        //1. 设置OPT2 选择内部高频HIRC振荡器
  SetOption(0xFFF2,0xe5ff);

  a=MC321_ReadByte(0XFFF0);
  a=MC321_ReadByte(0XFFF2);
  
  SetAddress(check_num);           //2．通过0x15命令设置校验个数
  InputData(0xFFFF);               //3．通过0x16命令输入校验初始值 
//  SetOption(0xFFF2,0xFEFF);      //设置OPT2 选择内部高频HIRC振荡器
  MC301_Instruct(CheMode);         //4．输入0x18命令进入校验模式
  a=0;
  while(!a)
  {
    a=GPIO_ReadInputDataBit(OTP_SDO_PORT,OTP_SDO_PIN);
  }
  Delay_1ms(1);
//  Delay_1us(10);
  a=MC321_PreRead(PDataRe);
  return a;
}
/*******************************************************************************
* Function Name  : MC32P7511_MODEL_IN
* Description    : 主芯片进模式和FPGA进模式
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void MC32P7511_MODEL_IN(u8 VddFlag)
{
	u8 temp;
	//power on & model in
	POWER_OFF(vpp00,vdd00);
	Delay_1ms(100);

	//input wave
	if (VddFlag==vdd65)
	{
		VDD65V_On;
		NOP;
		IO5V_On;
	} 
	else
	{
		VDD30V_On;
		IO5V_Off;
	}
        VDD_IO_Off; //VDD IO PIN SIGNAL

	NOP;
        NOP;
        NOP
        VDD_IO_On;

        //Delay_1ms(50);//武文君加  要求延时时间长一点 需要时间去加载0x8000值到0xc000地址
        
	CLN_VDD2GND_Off;
	VPP12V_On;
        VPP_IO_On;
        
        Delay_1ms(10);//李秀峰加  要求延时时间长一点 需要时间去加载0x8000值到0xc000地址
        
	Delay_100us(5);
    //Delay_100us(2);
        NOP;

	OTP_SDI_1;
	NOP;
	OTP_SDI_0;
	NOP;
	OTP_SDI_1;
	NOP;
	OTP_SDI_0;
	NOP;
	OTP_SDI_1;
	NOP;
	OTP_SDI_0;
	NOP;
	OTP_SDI_1;
	NOP;
	OTP_SDI_0;
	NOP;

	VPP12V_Off;
        VPP_IO_Off;
	CLN_VDD2GND_On;
	Delay_100us(5);
        NOP;

	OTP_SDI_1;
	NOP;
	OTP_SDI_0;
	NOP;

	CLN_VDD2GND_Off;
	VPP12V_On;
        VPP_IO_On;
	Delay_100us(5);
        NOP;

	OTP_SDI_1;
	NOP;
	OTP_SDI_0;
	NOP;
	OTP_SDI_1;
	NOP;
	OTP_SDI_0;
	NOP;
	VPP12V_Off;
        VPP_IO_Off;
	CLN_VDD2GND_On;
	Delay_100us(5);
        NOP;
        
	OTP_SDI_1;
	NOP;
	OTP_SDI_0;
	Delay_1ms(1);
        NOP;

}
void SetPTMmode(u16 mode)
{
  u8 i,temp;
  u16 a;
  MC301_Instruct(SetReadMode);
  for(i=0;i<16;i++)
  {
    a =mode>>i;
    if ((a & 0x0001)==1)
    {
      OTP_SDI_1;
    } 
    else
    {
      OTP_SDI_0;
    }
    OTP_SCK_1;  //1
    NOP;
    OTP_SCK_0;
    NOP;
  }   
}
u8 MC32P5222_MTPByteRead(u16 address)
{
  u8 a;
  SetAddress(address);           //1．通过0x15命令设置读取地址 
  SetPTMmode(ByteRead);          //2. 设PTM模式（指令码：0x1a ） Byte Read       
  a=MC321_R();                   //3. 数据读出（指令码：0x17）
  return a;
}
/*******************************************************************************
* Function Name  : MC32P5222_MTPByteRead_DiffMode
* Description    : MTP Page Write
* Input          : address:读取地址；pdin:   pdm:      
* Output         : None
* Return         : 
*******************************************************************************/
u8 MC32P5222_MTPByteRead_DiffMode(u16 address,u16 pdin,u16 ptm)
{
  u8 a;
  SetAddress(address);          //1．通过0x15命令设置读取地址 
  InputData(pdin);              //2.	数据输入（指令码：0x16）Program Margin1-3/Erase Margin1-3（见MTP模式切换操作）
  SetPTMmode(ptm);              //3.	设PTM模式（指令码：0x1a ）Program Margin1-3/Erase Margin1-3（见MTP模式切换操作）      
  a=MC321_R();                  //4. 数据读出（指令码：0x17）
  return a;
}
void MC32P5222_MTPWriteByte(u16 address,u8 wData)
{
  u8 j,temp;
  SetAddress(address);           //1．通过0x15命令设置读取地址 
  SetPTMmode(ByteWrite);          //2. 设PTM模式（指令码：0x1a ） Byte Read       
  InputData(wData);              //3.	数据输入（指令码：0x16）
//  wData=MC321_PreRead(PDataRe);
  MC301_Instruct(MTPwrite);      //4.	MTP烧写（指令码：0x1b）
  for(j=0;j<8;j++)
  {
    OTP_SCK_1;
    NOP;
    OTP_SCK_0;
    NOP;
  }  
  Delay_1ms(10);
  for(j=0;j<8;j++)
  {
    OTP_SCK_1;
    NOP;
    OTP_SCK_0;
    NOP;
  }    
}

/*******************************************************************************
* Function Name  : MC32P5222_MTPWritePage
* Description    : MTP Page Write
* Input          : PageStaAddress:页首地址；num:加载第几个数据；wData:加载的数据
* Output         : None
* Return         : 
*******************************************************************************/
void MC32P5222_MTPWritePage(u16 StaAddress,u8 num,u8 wData)
{
  u8 i,temp;
  if(num==0)//第一个数据
  {
    SetAddress(StaAddress);    //1．通过0x15命令设置页首地址 
    SetPTMmode(PageWrite);        //2. 设PTM模式（指令码：0x1a ） Page Write       
  }
  InputData(wData);               //3. 数据输入（指令码：0x16）
  MC301_Instruct(MTPLoad);        //4. MTP加载	
  for(i=0;i<16;i++)
  {
    OTP_SCK_1;
    NOP;
    OTP_SCK_0;
    NOP;
  }
  if(num<15)//不是最后一次
  {
    MC301_Instruct(AddrINC);      //5. 地址加一
  }
  else if(num==15)
  {
    MC301_Instruct(MTPwrite);     //6.MTP烧写
    for(i=0;i<8;i++)
    {
      OTP_SCK_1;
      NOP;
      OTP_SCK_0;
      NOP;
    }  
    Delay_1ms(5);
    for(i=0;i<8;i++)
    {
      OTP_SCK_1;
      NOP;
      OTP_SCK_0;
      NOP;
    }    
  }
}
//void MC32P5222_MTPWritePage(u16 PageStaAddress,u8 num,u8 wData)
//{
//  u8 i,j,temp;
//  u8 data[16]={0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf};
//  SetAddress(PageStaAddress);    //1．通过0x15命令设置页首地址 
//  SetPTMmode(PageWrite);        //2. 设PTM模式（指令码：0x1a ） Page Write 
//  for(i=0;i<16;i++)
//  {
//    wData=data[i];
//    InputData(wData);               //3. 数据输入（指令码：0x16）
//    MC301_Instruct(MTPLoad);        //4. MTP加载	
//    for(j=0;j<16;j++)
//    {
//      OTP_SCK_1;
//      NOP;
//      OTP_SCK_0;
//      NOP;
//    }
//    if(i<15)
//    {
//      MC301_Instruct(AddrINC);      //5. 地址加一
//    }
//    else if(i==15)
//    {
//      MC301_Instruct(MTPwrite);     //6.MTP烧写
//      for(j=0;j<8;j++)
//      {
//        OTP_SCK_1;
//        NOP;
//        OTP_SCK_0;
//        NOP;
//      }  
//      Delay_1ms(5);
//      for(j=0;j<8;j++)
//      {
//        OTP_SCK_1;
//        NOP;
//        OTP_SCK_0;
//        NOP;
//      }    
//    }
//  }
//}

/*******************************************************************************
* Function Name  : MC32P5222_MTPWriteChip
* Description    : MTP Chip Write
* Input          : num:加载第几个数据；wData:加载的数据
* Output         : None
* Return         : 
*******************************************************************************/

void MC32P5222_MTPWriteChip(u16 StaAddress,u8 num,u8 wData)
{
  u8 temp;
  if(num==0)//第一个数据
  {
    SetAddress(StaAddress);    //1．通过0x15命令设置页首地址 
    SetPTMmode(ChipWrite);        //2. 设PTM模式（指令码：0x1a ） Page Write       
  }
  InputData(wData);               //3. 数据输入（指令码：0x16）
  MC301_Instruct(MTPLoad);        //4. MTP加载	
  for(i=0;i<16;i++)
  {
    OTP_SCK_1;
    NOP;
    OTP_SCK_0;
    NOP;
  }
  if(num<15)//不是最后一次
  {
    MC301_Instruct(AddrINC);      //5. 地址加一
  }
  else if(num==15)
  {
    MC301_Instruct(MTPwrite);     //6.MTP烧写
    for(i=0;i<8;i++)
    {
      OTP_SCK_1;
      NOP;
      OTP_SCK_0;
      NOP;
    }  
    Delay_1ms(5);
    for(i=0;i<8;i++)
    {
      OTP_SCK_1;
      NOP;
      OTP_SCK_0;
      NOP;
    }    
  }
}

/*******************************************************************************
* Function Name  : MC32P5222_MTPPageErase
* Description    : MTP Page Erase
* Input          : PageStaAddress:页首地址
* Output         : None
* Return         : 
*******************************************************************************/
void MC32P5222_MTPPageErase(u16 StaAddress)
{
  u8 i,temp;
  SetAddress(StaAddress);    //1．通过0x15命令设置页首地址 
  SetPTMmode(PageErase);         //2. 设PTM模式（指令码：0x1a ） Page Write       
  MC301_Instruct(MTPwrite);      //3. MTP烧写
  for(i=0;i<8;i++)
  {
    OTP_SCK_1;
    NOP;
    OTP_SCK_0;
    NOP;
  }  
  Delay_1ms(10);
  for(i=0;i<8;i++)
  {
    OTP_SCK_1;
    NOP;
    OTP_SCK_0;
    NOP;
  }
}
/*******************************************************************************
* Function Name  : MC32P5222_MTPChipErase
* Description    : MTP Page Erase
* Input          : PageStaAddress:页首地址
* Output         : None
* Return         : 
*******************************************************************************/
void MC32P5222_MTPChipErase(u16 StaAddress)
{
  u8 i,temp;
  SetAddress(StaAddress);    //1．通过0x15命令设置页首地址 
  SetPTMmode(ChipErase);         //2. 设PTM模式（指令码：0x1a ） Chip Write       
  MC301_Instruct(MTPwrite);      //3. MTP烧写	
  for(i=0;i<8;i++)
  {
    OTP_SCK_1;
    NOP;
    OTP_SCK_0;
    NOP;
  }  
  Delay_1ms(10);
  for(i=0;i<8;i++)
  {
    OTP_SCK_1;
    NOP;
    OTP_SCK_0;
    NOP;
  }
}
//--------------------------------------------------------
u8 MC32P7511IRC_FPGA(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i;
  u8 mm=0x80;
  //u8 temp;
   u16 temp0_FT,temp1_FT ;
  //power on & model in
  MC32P7511_MODEL_IN(vdd30);
  Delay_1ms(1);
  MC301_Instruct(TestMod);

  for(i=0;i<5;i++)
  {
    MC321_IRC_INST(0X0000); //5 NOP
  }
  MC321_IRC_INST(0x3ccf) ; //movai	0xbf 
  MC321_IRC_INST(0x57f9) ; //movra	0x1f9
  MC321_IRC_INST(0x3cf8) ; //movai	0xf8
  MC321_IRC_INST(0x57fe) ; //movra	0x1fe
  MC321_IRC_INST(0x3c8f) ; //movai	0x8f
  MC321_IRC_INST(0x57fa) ; //movra	0x1fa  
  MC321_IRC_INST(0x3c88) ; //movai	0x88
  MC321_IRC_INST(0x57af) ; //movra	0x1af

  MC321_IRC_INST(0x3c01) ;  //movai	0x01 高频系统时钟由PSDO输出
  MC321_IRC_INST(0x57f8) ; 
  MC321_IRC_INST(0x0000) ;
  Delay_1ms(10);
  mm=0x80;
  OPTION_FRT=0; //initial 
  for(i=0;i<8;i++)
  {
    MC321FTtest(mm);
    if (IRC_VALUE == IRC_FreqType)
    {
      break;
    }
    if (IRC_VALUE<IRC_FreqType)
    {
      mm |=0x80 >>i;
    } 
    else
    {
      mm &=~(0x80>>i);
    }
    mm |=0x40>>i;
  }
  MC321FTtest(mm+1);
  temp0_FT=IRC_VALUE;

  MC321FTtest(mm);
  temp1_FT=IRC_VALUE;

  if((temp0_FT-IRC_FreqType)<(IRC_FreqType-temp1_FT))
  {
    mm=mm+1;
    IRC_VALUE=temp0_FT;
  }       
  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    OPTION_FRT=mm;        
    return 1;
  } 
  else
  {
    OPTION_FRT=0XFF;
    ERORR_VALUE=IRC_Value_false;
    return 0;
  }
}

//IRC校验测试版
void MC32P5222FTtest_11Bit(u16 osccal,u8 tadj)
{
  u16 temp;
  u8  adjH,adjL;
  u16 i;
  u16 cnt[3];
  //OPTION 1：
  //bit10:0  --  bit10:0   OSCCAL
  //bit15:11  --  bit4:0   TADJ  
  adjH=osccal/256; //H
  adjL=osccal%256; //L  
  
  temp=0x3c00+adjL;
  MC321_IRC_INST(temp);
  MC321_IRC_INST(0x57fb); //option1 L
  MC321_IRC_INST(0x0000);
  
  tadj<<=3;
  temp= (tadj&0xf8)|adjH;//TADJ不能改变
  temp=0x3c00+adjH;
  MC321_IRC_INST(temp); //
  MC321_IRC_INST(0x57fc); //option1 H  
  
  MC321_IRC_INST(0x0000);
  MC321_IRC_INST(0x0000);
  //----------------------------
  Delay_1ms(5);
  for (i=0;i<3;i++)
  {
    TIM_SetCounter(TIM3,0);
    TIM_SetCounter(TIM4,0);
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
    TIM_Cmd(TIM4,ENABLE);
    TIM_Cmd(TIM3,ENABLE);
    while(TIM_GetITStatus(TIM3,TIM_IT_Update) == RESET) ;
    cnt[i] = TIM_GetCounter(TIM4);
  }
  IRC_VALUE= (u16)((cnt[0]+cnt[1]+cnt[2])/3 );
} 
u8 MC32P5222IRC_FPGA(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i;
  u16 mm=0x400;
  //u8 temp;
   u16 temp0_FT,temp1_FT ;
  //power on & model in
  MC32P7511_MODEL_IN(vdd30);
  Delay_1ms(1);
  MC301_Instruct(TestMod);

  for(i=0;i<5;i++)
  {
    MC321_IRC_INST(0X0000); //5 NOP
  }
  
  //FCPU= 
  MC321_IRC_INST(0x3cef) ; //movai	
  MC321_IRC_INST(0x57f9) ; //movra	0x1f9
  
  //FAS=000
  MC321_IRC_INST(0x3cf8) ; //movai	0xf8
  MC321_IRC_INST(0x57fe) ; //movra	0x1fe
  
  //VLVRS=2.1V 
//  MC321_IRC_INST(0x3cbb) ; //movai	0xbb
//  MC321_IRC_INST(0x57fa) ; //movra	0x1fa  
  
  //LVDCR=0x88
//  MC321_IRC_INST(0x3c88) ; //movai	0x88
//  MC321_IRC_INST(0x57bd) ; //movra	0x1af

  MC321_IRC_INST(0x3c01) ;  //movai	0x01 高频系统时钟由PSDO输出
  MC321_IRC_INST(0x57f8) ; 
  MC321_IRC_INST(0x0000) ;
  Delay_1ms(800);
//  Delay_1ms(500);
  mm=0x400;
  OPTION_FRT=0; //initial 
  for(i=0;i<11;i++)
  {
    TADJ = 0xf8;
    MC32P5222FTtest_11Bit(mm,TADJ);
    if (IRC_VALUE == IRC_FreqType)
    {
      break;
    }
    if (IRC_VALUE<IRC_FreqType)
    {
      mm |=0x400 >>i;
    } 
    else
    {
      mm &=~(0x400>>i);
    }
    mm |=0x200>>i;
  }
  
  MC32P5222FTtest_11Bit(mm+1,TADJ);
  temp0_FT=IRC_VALUE;

  MC32P5222FTtest_11Bit(mm,TADJ);
  temp1_FT=IRC_VALUE;

  if((temp0_FT-IRC_FreqType)<(IRC_FreqType-temp1_FT))
  {
    mm=mm+1;
    IRC_VALUE=temp0_FT;
  }       
  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    OPTION_FRT=mm/256;   
    OPTION_FRT2=mm%256;
    return 1;
  } 
  else
  {
    OPTION_FRT=0xff;   
    OPTION_FRT2=0xff;
    ERORR_VALUE=IRC_Value_false;
    return 0;
  }
}
//烧写结束，IRC校验值验证
/*
u8 MC32P7011IRC_sure(u16 opt_data)
{
  u8 i;
  u8 optL,optH,opt2L,Fdata_H;
  u16 mm,Fdata,temp0_FT,temp1_FT;
  u8 OPTION_FAS;
  
  FM_CS_0;
  mm=0x400 ;//& (rdv<<8);
  //power on & model in
  MC321_MODEL_IN(vdd30);
  Delay_1ms(1);

  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID)
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  }  
  MC301_Instruct(TestMod);
  for(i=0;i<5;i++)
  {
    MC321_IRC_INST(0X0000); //5 NOP
  }  
  
  optL = opt_data%256;
  optH = opt_data/256;
  mm=0x2d00+optH;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x1f8b);     //写opt1 H  写温度调校位

  
  
  if (FMReadOne(Addr_Flash_OptionProtec+6)==1)//取flash中 option2 低字节
  {
    optL =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }    
  mm=0x2d00+optL;
  mm = mm | 0x30;//FDS=11
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x1f8c);     //写opt2 L

  MC321_IRC_INST(0x2d00);
  MC321_IRC_INST(0x1f89);     //写opt2 H  选择内部RC 
  
  OPTION_FAS = optL & 0x0E; //确定是16M 8M 4M 2M 1M 455K哪一种

  if(OPTION_FAS == 0x00)//16M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1239;
    IRC_FreqMax = 1261;  
    optL = 0x60;//opt2 L 分频系数64
  }
  else if(OPTION_FAS == 0x02)//8M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1239;
    IRC_FreqMax = 1261;   
    optL = 0x50;//opt2 L 分频系数64
  }
  else if(OPTION_FAS == 0x04)//4M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1239;
    IRC_FreqMax = 1261;     
    optL = 0x40;//opt2 L 分频系数32
  }
  else if(OPTION_FAS == 0x06)//2M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1239;
    IRC_FreqMax = 1261;    
    optL = 0x30;//opt2 L 分频系数16
  }   
  else if(OPTION_FAS == 0x08)//1M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1239;
    IRC_FreqMax = 1261;    
    optL = 0x20;//opt2 L 分频系数16
  }   
  else if(OPTION_FAS == 0x0A)//455K
  {
    IRC_FreqType = 2275;
    IRC_FreqMin = 2255;
    IRC_FreqMax = 2295;    
    optL = 0x00;//opt2 L 分频系数2
  }   
  else//上位机没有配置OPTION 
  {
      return 0;
  }
  mm=0x2d00+optL;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x1f88);     //写opt0 L   
  
  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;

  MC321_IRC_INST(0x2d01);     //movai 0x01
  MC321_IRC_INST(0x1f80);     //movra 0x80 //set HIRC output to SDO

  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;   

  MC32P7011FTtest_10Bit(mm);
  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    return 1;//IRC校准值 最终验证通过
  }
  return 0;
  //}校验结束
}
*/
u8 MC32P7511_Program()
{
  u8  ReCnt=0,op_addr=0;
  u16  Fdata,ROMReadData;
  //u8 temp;
  u8	FdataL,FdataH,option_shift=0;
  u8  ReFlag=0;
  u16  CheckSum=0;
  
  u16 mcu_addr;
 
  u16 CRC_value=0xFFFF;
  u16 i;
   //进模式 读MCU ID
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(5);
  //power on & model in
  if((DeviceConfig_xx.MCU_ID==0x7511)||(DeviceConfig_xx.MCU_ID==0x5222))
  {
    MC32P7511_MODEL_IN(vdd30);
  }
  CLN_VDD2GND_Off;
  VPP12V_On;
  VPP_IO_On;//FPGA VPP

  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID) 
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  } 

/*MTP读写、擦除指令验证
  ROMReadData=MC32P5222_MTPByteRead(0xc000);
  ROMReadData=MC32P5222_MTPByteRead(0xc0ff);
  ROMReadData=MC32P5222_MTPByteRead(0xc1ff);
  ROMReadData=MC32P5222_MTPByteRead(0xc2ff);
  ROMReadData=MC32P5222_MTPByteRead(0xc3ff); 
  
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc000,ProMargin1_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc0ff,ProMargin1_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc1ff,ProMargin1_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc2ff,ProMargin1_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc3ff,ProMargin1_PDIN,ProMargin_PTM);

  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc000,ProMargin2_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc0ff,ProMargin2_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc1ff,ProMargin2_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc2ff,ProMargin2_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc3ff,ProMargin2_PDIN,ProMargin_PTM);
  
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc000,ProMargin3_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc0ff,ProMargin3_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc1ff,ProMargin3_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc2ff,ProMargin3_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc3ff,ProMargin3_PDIN,ProMargin_PTM);  
  
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc000,EraMargin1_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc0ff,EraMargin1_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc1ff,EraMargin1_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc2ff,EraMargin1_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc3ff,EraMargin1_PDIN,EraMargin_PTM);

  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc000,EraMargin2_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc0ff,EraMargin2_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc1ff,EraMargin2_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc2ff,EraMargin2_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc3ff,EraMargin2_PDIN,EraMargin_PTM);
  
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc000,EraMargin3_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc0ff,EraMargin3_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc1ff,EraMargin3_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc2ff,EraMargin3_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc3ff,EraMargin3_PDIN,EraMargin_PTM);   
  
  //写byte
  MC32P5222_MTPWriteByte(0xc000,0x5a);
  
  ROMReadData=MC32P5222_MTPByteRead(0xc000);
  ROMReadData=MC32P5222_MTPByteRead(0xc0ff);
  ROMReadData=MC32P5222_MTPByteRead(0xc1ff);
  ROMReadData=MC32P5222_MTPByteRead(0xc2ff);
  ROMReadData=MC32P5222_MTPByteRead(0xc3ff); 
  
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc000,ProMargin1_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc0ff,ProMargin1_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc1ff,ProMargin1_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc2ff,ProMargin1_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc3ff,ProMargin1_PDIN,ProMargin_PTM);

  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc000,ProMargin2_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc0ff,ProMargin2_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc1ff,ProMargin2_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc2ff,ProMargin2_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc3ff,ProMargin2_PDIN,ProMargin_PTM);
  
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc000,ProMargin3_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc0ff,ProMargin3_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc1ff,ProMargin3_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc2ff,ProMargin3_PDIN,ProMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc3ff,ProMargin3_PDIN,ProMargin_PTM);  
  
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc000,EraMargin1_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc0ff,EraMargin1_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc1ff,EraMargin1_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc2ff,EraMargin1_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc3ff,EraMargin1_PDIN,EraMargin_PTM);

  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc000,EraMargin2_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc0ff,EraMargin2_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc1ff,EraMargin2_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc2ff,EraMargin2_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc3ff,EraMargin2_PDIN,EraMargin_PTM);
  
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc000,EraMargin3_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc0ff,EraMargin3_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc1ff,EraMargin3_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc2ff,EraMargin3_PDIN,EraMargin_PTM);
  ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc3ff,EraMargin3_PDIN,EraMargin_PTM);    
  
  //写page
  for(i=0;i<16;i++)
  {
    MC32P5222_MTPWritePage(0xc000,i,i);
  }
  mcu_addr = MC321_PreRead(PAddrRe);
  
  ROMReadData=MC32P5222_MTPByteRead(0xc000);
  ROMReadData=MC32P5222_MTPByteRead(0xc001);
  ROMReadData=MC32P5222_MTPByteRead(0xc002);
  ROMReadData=MC32P5222_MTPByteRead(0xc003);
  ROMReadData=MC32P5222_MTPByteRead(0xc004);
  ROMReadData=MC32P5222_MTPByteRead(0xc005);
  ROMReadData=MC32P5222_MTPByteRead(0xc006);
  ROMReadData=MC32P5222_MTPByteRead(0xc007);
  ROMReadData=MC32P5222_MTPByteRead(0xc008);
  ROMReadData=MC32P5222_MTPByteRead(0xc009);
  ROMReadData=MC32P5222_MTPByteRead(0xc00a);
  ROMReadData=MC32P5222_MTPByteRead(0xc00b);
  ROMReadData=MC32P5222_MTPByteRead(0xc00c);
  ROMReadData=MC32P5222_MTPByteRead(0xc00d);
  ROMReadData=MC32P5222_MTPByteRead(0xc00e);
  ROMReadData=MC32P5222_MTPByteRead(0xc00f);
  ROMReadData=MC32P5222_MTPByteRead(0xc010);
  ROMReadData=MC32P5222_MTPByteRead(0xc01f);    
  ROMReadData=MC32P5222_MTPByteRead(0xc1ff);
  ROMReadData=MC32P5222_MTPByteRead(0xc3ff); 
  
  //擦page
  MC32P5222_MTPPageErase(0xc000);
  
  ROMReadData=MC32P5222_MTPByteRead(0xc000);
  ROMReadData=MC32P5222_MTPByteRead(0xc001);
  ROMReadData=MC32P5222_MTPByteRead(0xc002);
  ROMReadData=MC32P5222_MTPByteRead(0xc003);
  ROMReadData=MC32P5222_MTPByteRead(0xc004);
  ROMReadData=MC32P5222_MTPByteRead(0xc005);
  ROMReadData=MC32P5222_MTPByteRead(0xc006);
  ROMReadData=MC32P5222_MTPByteRead(0xc007);
  ROMReadData=MC32P5222_MTPByteRead(0xc008);
  ROMReadData=MC32P5222_MTPByteRead(0xc009);
  ROMReadData=MC32P5222_MTPByteRead(0xc00a);
  ROMReadData=MC32P5222_MTPByteRead(0xc00b);
  ROMReadData=MC32P5222_MTPByteRead(0xc00c);
  ROMReadData=MC32P5222_MTPByteRead(0xc00d);
  ROMReadData=MC32P5222_MTPByteRead(0xc00e);
  ROMReadData=MC32P5222_MTPByteRead(0xc00f);
  ROMReadData=MC32P5222_MTPByteRead(0xc010);
  ROMReadData=MC32P5222_MTPByteRead(0xc01f);    
  ROMReadData=MC32P5222_MTPByteRead(0xc1ff);
  ROMReadData=MC32P5222_MTPByteRead(0xc3ff); 
  
  //写chip
  for(i=0;i<16;i++)
  {
    MC32P5222_MTPWriteChip(0xc000,i,i);
  }    
  mcu_addr = MC321_PreRead(PAddrRe);
  
  ROMReadData=MC32P5222_MTPByteRead(0xc000);
  ROMReadData=MC32P5222_MTPByteRead(0xc001);
  ROMReadData=MC32P5222_MTPByteRead(0xc002);
  ROMReadData=MC32P5222_MTPByteRead(0xc003);
  ROMReadData=MC32P5222_MTPByteRead(0xc004);
  ROMReadData=MC32P5222_MTPByteRead(0xc005);
  ROMReadData=MC32P5222_MTPByteRead(0xc006);
  ROMReadData=MC32P5222_MTPByteRead(0xc007);
  ROMReadData=MC32P5222_MTPByteRead(0xc008);
  ROMReadData=MC32P5222_MTPByteRead(0xc009);
  ROMReadData=MC32P5222_MTPByteRead(0xc00a);
  ROMReadData=MC32P5222_MTPByteRead(0xc00b);
  ROMReadData=MC32P5222_MTPByteRead(0xc00c);
  ROMReadData=MC32P5222_MTPByteRead(0xc00d);
  ROMReadData=MC32P5222_MTPByteRead(0xc00e);
  ROMReadData=MC32P5222_MTPByteRead(0xc00f);
  ROMReadData=MC32P5222_MTPByteRead(0xc010);
  ROMReadData=MC32P5222_MTPByteRead(0xc011);
  ROMReadData=MC32P5222_MTPByteRead(0xc012);
  ROMReadData=MC32P5222_MTPByteRead(0xc013);
  ROMReadData=MC32P5222_MTPByteRead(0xc014);
  ROMReadData=MC32P5222_MTPByteRead(0xc015);
  ROMReadData=MC32P5222_MTPByteRead(0xc016);
  ROMReadData=MC32P5222_MTPByteRead(0xc017);
  ROMReadData=MC32P5222_MTPByteRead(0xc018);
  ROMReadData=MC32P5222_MTPByteRead(0xc019);
  ROMReadData=MC32P5222_MTPByteRead(0xc01a);
  ROMReadData=MC32P5222_MTPByteRead(0xc01b);
  ROMReadData=MC32P5222_MTPByteRead(0xc01c);
  ROMReadData=MC32P5222_MTPByteRead(0xc01d);
  ROMReadData=MC32P5222_MTPByteRead(0xc01e);
  ROMReadData=MC32P5222_MTPByteRead(0xc01f);  
  ROMReadData=MC32P5222_MTPByteRead(0xc1ff);
  ROMReadData=MC32P5222_MTPByteRead(0xc3ff); 
  
  //擦chip
  MC32P5222_MTPChipErase(0xc000);  
  
  ROMReadData=MC32P5222_MTPByteRead(0xc000);
  ROMReadData=MC32P5222_MTPByteRead(0xc001);
  ROMReadData=MC32P5222_MTPByteRead(0xc002);
  ROMReadData=MC32P5222_MTPByteRead(0xc003);
  ROMReadData=MC32P5222_MTPByteRead(0xc004);
  ROMReadData=MC32P5222_MTPByteRead(0xc005);
  ROMReadData=MC32P5222_MTPByteRead(0xc006);
  ROMReadData=MC32P5222_MTPByteRead(0xc007);
  ROMReadData=MC32P5222_MTPByteRead(0xc008);
  ROMReadData=MC32P5222_MTPByteRead(0xc009);
  ROMReadData=MC32P5222_MTPByteRead(0xc00a);
  ROMReadData=MC32P5222_MTPByteRead(0xc00b);
  ROMReadData=MC32P5222_MTPByteRead(0xc00c);
  ROMReadData=MC32P5222_MTPByteRead(0xc00d);
  ROMReadData=MC32P5222_MTPByteRead(0xc00e);
  ROMReadData=MC32P5222_MTPByteRead(0xc00f);
  ROMReadData=MC32P5222_MTPByteRead(0xc010);
  ROMReadData=MC32P5222_MTPByteRead(0xc011);
  ROMReadData=MC32P5222_MTPByteRead(0xc012);
  ROMReadData=MC32P5222_MTPByteRead(0xc013);
  ROMReadData=MC32P5222_MTPByteRead(0xc014);
  ROMReadData=MC32P5222_MTPByteRead(0xc015);
  ROMReadData=MC32P5222_MTPByteRead(0xc016);
  ROMReadData=MC32P5222_MTPByteRead(0xc017);
  ROMReadData=MC32P5222_MTPByteRead(0xc018);
  ROMReadData=MC32P5222_MTPByteRead(0xc019);
  ROMReadData=MC32P5222_MTPByteRead(0xc01a);
  ROMReadData=MC32P5222_MTPByteRead(0xc01b);
  ROMReadData=MC32P5222_MTPByteRead(0xc01c);
  ROMReadData=MC32P5222_MTPByteRead(0xc01d);
  ROMReadData=MC32P5222_MTPByteRead(0xc01e);
  ROMReadData=MC32P5222_MTPByteRead(0xc01f);  
  ROMReadData=MC32P5222_MTPByteRead(0xc1ff);
  ROMReadData=MC32P5222_MTPByteRead(0xc3ff); 
*/
  
/*OTP 修改指令验证  
  ROMReadData=MC321_ReadByte(0xFFF0);
  ROMReadData=MC321_ReadByte(0xFFF1);
  ROMReadData=MC321_ReadByte(0xFFF2);
  ROMReadData=MC321_ReadByte(0xFFF3);
  ROMReadData=MC321_ReadByte(0x8000);
  ROMReadData=MC321_ReadByte(0x8001);
  ROMReadData=MC321_ReadByte(0x8002);
  ROMReadData=MC321_ReadByte(0x8003);
  SetOption(0xFFF0,0x55aa);        //
  SetOption(0xFFF1,0x55aa);        //
  SetOption(0xFFF2,0x55aa);        //
  SetOption(0xFFF3,0x55aa);        //  
  MC321_WriteByte(0x8000,0xaa55);        //
  MC321_WriteByte(0x8001,0xaa55);        //
  MC321_WriteByte(0x8002,0xaa55);        //
  MC321_WriteByte(0x8003,0xaa55);        //  
  ROMReadData=MC321_ReadByte(0xFFF0);
  ROMReadData=MC321_ReadByte(0xFFF1);
  ROMReadData=MC321_ReadByte(0xFFF2);
  ROMReadData=MC321_ReadByte(0xFFF3);  
  ROMReadData=MC321_ReadByte(0x8000);
  ROMReadData=MC321_ReadByte(0x8001);
  ROMReadData=MC321_ReadByte(0x8002);
  ROMReadData=MC321_ReadByte(0x8003);  
*/
  
  FMReadOne(0x012120+8); 
  if(!(Rxdata&0x01))//MTP 烧写
  {
    FM_CS_0;
    FMReadMore(Addr_Flash_ROMStart+0x8000-1);    
    for(i=0;i<1024;i++)
    {
      SPI_ReadByte();
      FdataL=Rxdata;
//      CheckSum +=FdataL;
      SPI_ReadByte();
      FdataH=Rxdata;
//      CheckSum +=FdataH;
      MC32P5222_MTPWriteByte(0xc000+i,FdataL);
      ROMReadData=MC32P5222_MTPByteRead(0xc000+i);
      if(ROMReadData!=FdataL)
      {
        ERORR_VALUE=MTP_Write_false;
        return 0;
      }
    }
    VDD30V_Off;
    POWER_OFF(vpp00,vdd00);
    return 1;  
  }
  if(DeviceConfig_xx.MCU_ID==0x7511)
  {
    //读芯片 是否写过 
    FM_CS_0;
    
    ROMReadData=MC321_ReadByte(0x8000);
    ROMReadData &= 0x0007;
    
    FMReadOne(0x012120+0); 
    Rxdata&=0x07;

    if(ROMReadData == 3)//用过 0.5K字节MTP模式，第一次烧录
    {
      if(Rxdata==0x04)//想再用 1K字节MTP模式 直接报错
      {
        ERORR_VALUE=OPTION_Write_false;
        return 0; 
      }    
    }
    else if(ROMReadData == 2)//用过 0.5K字节MTP模式，第二次烧录 
    {
      if(Rxdata==0x05||Rxdata==0x03)//想再用 1K字节MTP模式  或0.5K字节MTP模式，第一次烧录  直接报错
      {
        ERORR_VALUE=OPTION_Write_false;
        return 0; 
      }  
    }
    else if(ROMReadData == 5)//用过 1K字节MTP模式
    {
      if(Rxdata==0x03||Rxdata==0x02)//想再用 0.5K字节MTP模式  直接报错
      {
        ERORR_VALUE=OPTION_Write_false;
        return 0; 
      }    
    }   
  }
  
  //IRC adjust
  if (DeviceConfig_xx.IRC_Select==0x01)//正式代码
//  if (DeviceConfig_xx.IRC_Select==0x00)//调试代码
  {
    if (DeviceConfig_xx.MCU_ID==0x7511)
    {
      ReFlag=MC32P7511IRC_FPGA(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
    }
    else if(DeviceConfig_xx.MCU_ID==0x5222)
    {
      ReFlag=MC32P5222IRC_FPGA(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
    }
    if(ReFlag == 0) 
    {
      ERORR_VALUE=IRC_Value_false;
      return 0;
    }
  }    

   //进模式 读MCU ID
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(5);
  //power on & model in
  if((DeviceConfig_xx.MCU_ID==0x7511)||(DeviceConfig_xx.MCU_ID==0x5222))
  {
    MC32P7511_MODEL_IN(vdd30);
  }	
  CLN_VDD2GND_Off;
  VPP12V_On;
  VPP_IO_On;//FPGA VPP

  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID) 
  {
          ERORR_VALUE=OTP_ModeIn_false;
          return 0;
  }      
  
  if (DeviceConfig_xx.MCU_ID==0x7511)
  {
    //Write Page
    FMReadOne(0x012120+0);
    FdataL = Rxdata;  
    FMReadOne(0x012120+1);
    FdataH = Rxdata;  
    Fdata=FdataH*0x100+FdataL;  
    ReCnt = 0x00;
  //  Fdata = 0xFFF7;//只写bit4:3 不写PAGE 
    
    
    while(++ReCnt<6)
    {		
      MC321_WriteByte(0x8000,Fdata);
  //    MC321_WriteByte(0x8001,0xFFFF);
      for(i=0;i<3;i++)
      {
        ROMReadData=MC321_ReadByte_DiffMode(i,0x8000);
        if (ROMReadData==Fdata)
        {
        }
        else
        {
          if(i==0)
          {
            ERORR_VALUE=NOMALL_ERROR;
          }
          else if(i==1)
          {
            ERORR_VALUE=MAGRIN1_ERROR;
          }
          else if(i==2)
          {
            ERORR_VALUE=OFF_MAGRIN_ERROR;
          }
          break;
        }
      } 
      if(i==3)//3种模式读取都和写入的数据一致 烧写成功 1
      {
        ReCnt=0;
        break;
      }
      else if (ReCnt==4)
      {
        OTP_ADDR=0x8000;
      //  ERORR_VALUE=OPTION_Write_false;
        if(ERORR_VALUE!=NOMALL_ERROR&&ERORR_VALUE!=MAGRIN1_ERROR&&ERORR_VALUE!=OFF_MAGRIN_ERROR)
        {             
          ERORR_VALUE=OPTION_Write_false;
        }       
        return 0;
      }                 
    }     

    if(FdataL == 0xF2)  //the secand write 0.5k
    {
      option_shift = 0;
      DeviceConfig_xx.RomEndAddr = 0x7ff;
    }
    else if(FdataL == 0xF3)  //the first write 0.5k
    {
      option_shift = 4;
      DeviceConfig_xx.RomEndAddr = 0x7ff;
    }                
    else if(FdataL == 0xF5)//write 1k
    {
      option_shift = 0;
      DeviceConfig_xx.RomEndAddr = 0xfff;
    }
    else
    {
      return 0;
    }                             
  }
  
  //再进模式
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(5);
  //power on & model in
  if((DeviceConfig_xx.MCU_ID==0x7511)||(DeviceConfig_xx.MCU_ID==0x5222))
  {
    MC32P7511_MODEL_IN(vdd30);
  }	
  CLN_VDD2GND_Off;
  VPP12V_On;
  VPP_IO_On;//FPGA VPP

  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID) 
  {
          ERORR_VALUE=OTP_ModeIn_false;
          return 0;
  }      
  
   //Write ROM
  if (DeviceConfig_xx.WriteEn & 0x0f)
  {
    CheckSum=0;
    FM_CS_0;
    FMReadMore(Addr_Flash_ROMStart+DeviceConfig_xx.RomFirAddr-1);
    Fdata =Rxdata;

    
    ROMReadData=MC321_ReadByte(0x0000); //set ADDR

    for(OTP_ADDR=DeviceConfig_xx.RomFirAddr;OTP_ADDR<=DeviceConfig_xx.RomEndAddr;)
    {
      SPI_ReadByte();
      FdataL=Rxdata;
      CheckSum +=FdataL;
      SPI_ReadByte();
      FdataH=Rxdata;
      CheckSum +=FdataH;
      
      Fdata=FdataH*0x100+FdataL;
      
      //计算CRC
      CRC_value=CountCRC(FdataH,FdataL,CRC_value);//
      
      //Fdata=Fdata & 0x3fff;
      //2012.09.12 dynamic id
      mcu_addr=OTP_ADDR/2;
      
      OTP_ADDR=OTP_ADDR+2;						
      //------------------------------------------------
      Rxdata=Fdata;
      
      ReCnt=0;
      while(++ReCnt<6)  //start wr rom
      {	
        if (Fdata!=0xffff)
        {
          MC321_W(Fdata); //wr data to rom
        }			
        for(i=0;i<3;i++)
        {
          ROMReadData=MC321_R_DiffMode(i);
          if (ROMReadData==Fdata)
          {
          }
          else
          {
            if(i==0)
            {
              ERORR_VALUE=NOMALL_ERROR;
            }
            else if(i==1)
            {
              ERORR_VALUE=MAGRIN1_ERROR;
            }
            else if(i==2)//测试用
            {
              ERORR_VALUE=OFF_MAGRIN_ERROR;
            }
            break;
          }
        } 
        if(i==3)//3种模式读取都和写入的数据一致 烧写成功 2
        {
          ReCnt=0;
          break;
        }
        else if (ReCnt==4)
        {
        //  ERORR_VALUE=ROM_Write_false;
          if(ERORR_VALUE!=NOMALL_ERROR&&ERORR_VALUE!=MAGRIN1_ERROR&&ERORR_VALUE!=OFF_MAGRIN_ERROR)
          {             
            ERORR_VALUE=ROM_Write_false;
          }   
          OTP_ADDR=mcu_addr;
          return 0;
        }               
      }
      if (OTP_ADDR<DeviceConfig_xx.RomEndAddr)
      {
        MC301_Instruct(AddrINC);
        //ROMReadData=MC321_ReadByte(0x0000); //set ADDR
      }
    }
    FM_CS_1;
  }
  
  
//  ROMReadData=MC321_ReadByte(0xFFF0);
//  ROMReadData=MC321_ReadByte(0xFFF1);
//  ROMReadData=MC321_ReadByte(0xFFF2);
//  ROMReadData=MC321_ReadByte(0xFFF3);
//  ROMReadData=MC321_ReadByte(0x8000);
//  ROMReadData=MC321_ReadByte(0x8001);
//  ROMReadData=MC321_ReadByte(0x8002);
//  ROMReadData=MC321_ReadByte(0x8003);
//  SetOption(0xFFF0,0x55aa);        //
//  SetOption(0xFFF1,0x55aa);        //
//  SetOption(0xFFF2,0x55aa);        //
//  SetOption(0xFFF3,0x55aa);        //  
//  MC321_WriteByte(0x8000,0xaa55);        //
//  MC321_WriteByte(0x8001,0xaa55);        //
//  MC321_WriteByte(0x8002,0xaa55);        //
//  MC321_WriteByte(0x8003,0xaa55);        //  
//  ROMReadData=MC321_ReadByte(0xFFF0);
//  ROMReadData=MC321_ReadByte(0xFFF1);
//  ROMReadData=MC321_ReadByte(0xFFF2);
//  ROMReadData=MC321_ReadByte(0xFFF3);  
//  ROMReadData=MC321_ReadByte(0x8000);
//  ROMReadData=MC321_ReadByte(0x8001);
//  ROMReadData=MC321_ReadByte(0x8002);
//  ROMReadData=MC321_ReadByte(0x8003);  
   
//-------------------------以下写OPTION相关代码----------------------- 
  //Write OPTION  
  if(DeviceConfig_xx.WriteEn & 0xf0)
  {
    FM_CS_0;
    if(DeviceConfig_xx.OptionAddr==0x8000)
    {
      FMReadMore(Addr_Flash_OptionProtec-1);
    }
    else if(DeviceConfig_xx.OptionAddr==0x8001)
    {
      FMReadMore(Addr_Flash_OptionProtec+1);
    }
    Fdata =Rxdata;
    
    for(op_addr=0;op_addr<DeviceConfig_xx.OptionSize;op_addr++)//
    {
      SPI_ReadByte();//从flash中取一个字节
      FdataL=Rxdata;
      CheckSum +=FdataL;
      SPI_ReadByte();//再从flash中取一个字节
      FdataH=Rxdata;
      CheckSum +=FdataH;
      Fdata=FdataH*0x100+FdataL;//组成一个字
      
      if(DeviceConfig_xx.MCU_ID==0x7511)//
      {
        if(op_addr==1)
        {
          ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);
          if((ROMReadData&0xff)==0xff)//没写过频率校准值
          {
            Fdata = (Fdata&0xff00)|OPTION_FRT;
          }
          else
          {
            Fdata = (Fdata&0xff00) | (ROMReadData&0xff);
          }        
        }
      }
      if(DeviceConfig_xx.MCU_ID==0x5222)//
      {
        if(op_addr==1)
        {
          ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);
          if((ROMReadData&0x3ff)==0x3ff)//没写过频率校准值
          {
            Fdata=TADJ;
            Fdata<<=11;
            Fdata = (Fdata&0xf800)|(OPTION_FRT<<8)|OPTION_FRT2;
          }
          else
          {
            Fdata = ROMReadData;
          }   
        }
        else if(op_addr==2)
        {
          TADJ>>=5;
          TADJ&=0x01;
          Fdata= (Fdata&0xfe)|TADJ;
        }
      }
      ReCnt=0;
      while(++ReCnt<6)
      {		
        MC321_WriteByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift,Fdata);
        for(i=0;i<3;i++)
        {
          ROMReadData=MC321_ReadByte_DiffMode(i,DeviceConfig_xx.OptionAddr+op_addr+option_shift);
          if (ROMReadData==Fdata)
          {
          }
          else
          {
            if(i==0)
            {
              ERORR_VALUE=NOMALL_ERROR;
            }
            else if(i==1)
            {
              ERORR_VALUE=MAGRIN1_ERROR;
            }
            else if(i==2)
            {
              ERORR_VALUE=OFF_MAGRIN_ERROR;
            }
            break;
          }
        } 
        if(i==3)//3种模式读取都和写入的数据一致 烧写成功 4
        {
          ReCnt=0;
          break;
        }
        else if (ReCnt==4)
        {
        //  ERORR_VALUE=OPTION_Write_false;
          if(ERORR_VALUE!=NOMALL_ERROR&&ERORR_VALUE!=MAGRIN1_ERROR&&ERORR_VALUE!=OFF_MAGRIN_ERROR)
          {             
            ERORR_VALUE=OPTION_Write_false;
          }       
          OTP_ADDR=DeviceConfig_xx.OptionAddr;
          return 0;
        }                     
      }                         
    }
    FM_CS_1;
  }

  //Hardware CheckSum
  /*
  ROMReadData = GetCheckSum(mcu_addr+1);
  if(ROMReadData!=CRC_value)
  {
    ERORR_VALUE=ROM_Write_false;
    OTP_ADDR= mcu_addr;
    return 0;    
  }
  */
  //write checksum
  ReCnt=0;  
  while(++ReCnt<3)
  {
    if(DeviceConfig_xx.MCU_ID==0x7511)
    {
      if(option_shift==0x00)//write 1k or the secand write 0.5k
      {
        MC321_WriteByte(0x800c,CheckSum);
      }
      else if(option_shift==0x04)//the first write 0.5k
      {
        MC321_WriteByte(0x800d,CheckSum);
      }
    }
    else if(DeviceConfig_xx.MCU_ID==0x5222)
    {
      MC321_WriteByte(0x800f,CheckSum);
    }
  }
  
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  return 1;  
}