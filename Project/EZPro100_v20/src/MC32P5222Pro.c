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
#include "MC32P5222Pro.h"
//#include "MC32P7511.h"
#include "MC9033.h"

//uint8_t data[]= {
//    0x28,0x10,
//    };
//uint16_t crc;
//uint8_t i, chChar;
//uint16_t wCRC = 0xFFFF;

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
//  SetOption(0xFFF0,0x0000);        
//  SetOption(0xFFF1,0x21eb);        
//  SetOption(0xFFF2,0x06c0); 

  a=MC321_ReadByte(0XFFF0);
  a=MC321_ReadByte(0XFFF1);  
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
* Function Name  : MC32P5222_MODEL_IN
* Description    : 主芯片进模式和FPGA进模式
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void MC32P5222_MODEL_IN(u8 VddFlag)
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
        
        if(DeviceConfig_xx.MCU_ID==0x7511)
        {
          Delay_1ms(1);//李秀峰加  要求延时时间长一点 需要时间去加载0x8000值到0xc000地址
        }
        else if(DeviceConfig_xx.MCU_ID==0x5222)
        {
          Delay_1ms(10);
        }
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
  Delay_1ms(12);
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
    Delay_1ms(6);
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
* Function Name  : MC32P5222_MTPWriteChip
* Description    : MTP Chip Write
* Input          : num:加载第几个数据；wData:加载的数据
* Output         : None
* Return         : 
*******************************************************************************/

void MC32P5222_MTPWriteChip(u16 StaAddress,u8 num,u8 wData)
{
  u8 i,temp;
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

void MC32P5222FT_11Bit(u8 optL,u8 optH)
{
  u16 i;
  u16 cnt[3];
  //OPTION 1：
  //bit10:0  --  bit10:0   OSCCAL
  //bit15:11  --  bit4:0   TADJ  

  MC321_IRC_INST(0x3c00+optL);
  MC321_IRC_INST(0x57fb); //option1 L 

  MC321_IRC_INST(0x3c00+optH);
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

u8 MC32P5222IRC(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i,j;
  u16 mm=0x400;
  u8 optH,optL,tempH;
  u16 temp0_FT,temp1_FT ;
  u8 OPTION_TEMPADJ[3];
   
  MC301_Instruct(TestMod);

  for(i=0;i<5;i++)
  {
    MC321_IRC_INST(0X0000); //5 NOP
  }
  
  MC321_IRC_INST(0x3c00) ; //movai	
  MC321_IRC_INST(0x57f9) ; //movra	0x1f9  
  MC321_IRC_INST(0x3c00) ; //movai	
  MC321_IRC_INST(0x57fa) ; //movra	0x1f9 
  MC321_IRC_INST(0x3c00) ; //movai	
  MC321_IRC_INST(0x57fb) ; //movra	0x1f9  
  MC321_IRC_INST(0x3c00) ; //movai	
  MC321_IRC_INST(0x57fc) ; //movra	0x1f9  
  MC321_IRC_INST(0x3c00) ; //movai	
  MC321_IRC_INST(0x57fd) ; //movra	0x1f9 
  MC321_IRC_INST(0x3c00) ; //movai	
  MC321_IRC_INST(0x57fe) ; //movra	0x1f9 
  MC321_IRC_INST(0x3c00) ; //movai	
  MC321_IRC_INST(0x57ff) ; //movra	0x1f9 
  MC321_IRC_INST(0x3c00) ; //movai	
  MC321_IRC_INST(0x57f7) ; //movra	0x1f9 
  
  if (FMReadOne(Addr_Flash_OptionProtec+5)==1)//取option 2 高字节 确定是16M 8M 4M 2M 1M 455K哪一种
  {
    optH =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    return 0;
  }    
  //FAS=? FDA=? RCSMTB=?
  optH &= 0x3f;
  MC321_IRC_INST(0x3c00+optH) ; //movai	0xf8
  MC321_IRC_INST(0x57fe) ; //movra	0x1fe  
  
  if((optH&0x07)==0x00)//16M
  {
    tempH=0x7c;
    OPTION_TEMPADJ[0]=0x0a;//0x0a
    OPTION_TEMPADJ[1]=0x09;//0x09
    OPTION_TEMPADJ[2]=0x0b;//0x0b
  }
  else if((optH&0x07)==0x01)//8M
  {
    tempH=0x6c;
    OPTION_TEMPADJ[0]=0x12;//0x12
    OPTION_TEMPADJ[1]=0x11;//0x11
    OPTION_TEMPADJ[2]=0x13;//0x13
  }
  else if((optH&0x07)==0x02)//4M
  {
    tempH=0x5c;
    OPTION_TEMPADJ[0]=0x29;//
    OPTION_TEMPADJ[1]=0x2a;
    OPTION_TEMPADJ[2]=0x28;//
  }
  else if((optH&0x07)==0x03)//2M
  {
    tempH=0x4c;
    OPTION_TEMPADJ[0]=0x3f;//
    OPTION_TEMPADJ[1]=0x3e;
    OPTION_TEMPADJ[2]=0x3d;//    
  }
  else if((optH&0x07)==0x04)//1M
  {
    tempH=0x3c;
    OPTION_TEMPADJ[0]=0x3f;//
    OPTION_TEMPADJ[1]=0x3e;
    OPTION_TEMPADJ[2]=0x3d;//  
  }
  else if((optH&0x07)==0x05)//455K
  {
    tempH=0x1c;
    OPTION_TEMPADJ[0]=0x3f;//
    OPTION_TEMPADJ[1]=0x3e;
    OPTION_TEMPADJ[2]=0x3d;//      
  }
  else
  {
    return 0;
  }
  //FCPU=?
  MC321_IRC_INST(0x3c00+tempH) ; //movai	
  MC321_IRC_INST(0x57f9) ; //movra	0x1f9
  

  if (FMReadOne(Addr_Flash_OptionProtec+4)==1)//取option 2 低字节 
  {
    optL =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    return 0;
  }    
  //VDSEL=? TADJH=?
  optL &= 0x03;
  MC321_IRC_INST(0x3c00+optL) ; //movai	0xf8
  MC321_IRC_INST(0x57fd) ; //movra	0x1fe

  MC321_IRC_INST(0x3c01) ;  //movai	0x01 高频系统时钟由PSDO输出
  MC321_IRC_INST(0x57f8) ; 
  MC321_IRC_INST(0x0000) ;
    
  for(j=0;j<3;j++)
  {
    mm=0x400;
    OPTION_FRT=0xff;   
    OPTION_FRT2=0xff;
    OPTION_TEMPADJ[j]<<=3;
    for(i=0;i<11;i++)
    {
      optH=mm/256; 
      optL=mm%256;  
      optH=(optH&0x07)|(OPTION_TEMPADJ[j]&0xf8);
      MC32P5222FT_11Bit(optL,optH);
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
   
    optH=(mm+1)/256; 
    optL=(mm+1)%256;  
    optH=(optH&0x07)|(OPTION_TEMPADJ[j]&0xf8);
    MC32P5222FT_11Bit(optL,optH);
    temp0_FT=IRC_VALUE;

    optH=mm/256; 
    optL=mm%256;  
    optH=(optH&0x07)|(OPTION_TEMPADJ[j]&0xf8); 
    MC32P5222FT_11Bit(optL,optH);
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
      OPTION_FRT=OPTION_FRT|(optH&0xf8);
      return 1;
    } 
  }
  OPTION_FRT=0xff;   
  OPTION_FRT2=0xff;
  ERORR_VALUE=IRC_Value_false;
  return 0;
}

u8 MC32P5222IRC_Check(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 optH,optL;

  if (FMReadOne(Addr_Flash_OptionProtec+4)==1)//取option 2 低字节 
  {
    optL =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    return 0;
  }    
  //VDSEL=? TADJH=?
  optL &= 0x03;
  MC321_IRC_INST(0x3c00+optL) ; //movai	0xf8
  MC321_IRC_INST(0x57fd) ; //movra	0x1fe
  
  optL=OPTION_FRT2;
  optH=OPTION_FRT;

  MC32P5222FT_11Bit(optL,optH);
            
  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    //OPTION_FRT=mm/256;   
    //OPTION_FRT2=mm%256;
    //OPTION_FRT=OPTION_FRT|(optH&0xf8);
    return 1;
  } 
  OPTION_FRT=0xff;   
  OPTION_FRT2=0xff;
  ERORR_VALUE=IRC_Value_false;
  return 0;
}

u8 MC32P5222IRC_ModeInCheck(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i;
  u8 optH,optL,tempH;
//  u8 OPTION_TEMPADJ[3];
   
  MC301_Instruct(TestMod);

  for(i=0;i<5;i++)
  {
    MC321_IRC_INST(0X0000); //5 NOP
  }
  
  MC321_IRC_INST(0x3c00) ; //movai	
  MC321_IRC_INST(0x57f9) ; //movra	0x1f9  
  MC321_IRC_INST(0x3c00) ; //movai	
  MC321_IRC_INST(0x57fa) ; //movra	0x1f9 
  MC321_IRC_INST(0x3c00) ; //movai	
  MC321_IRC_INST(0x57fb) ; //movra	0x1f9  
  MC321_IRC_INST(0x3c00) ; //movai	
  MC321_IRC_INST(0x57fc) ; //movra	0x1f9  
  MC321_IRC_INST(0x3c00) ; //movai	
  MC321_IRC_INST(0x57fd) ; //movra	0x1f9 
  MC321_IRC_INST(0x3c00) ; //movai	
  MC321_IRC_INST(0x57fe) ; //movra	0x1f9 
  MC321_IRC_INST(0x3c00) ; //movai	
  MC321_IRC_INST(0x57ff) ; //movra	0x1f9 
  MC321_IRC_INST(0x3c00) ; //movai	
  MC321_IRC_INST(0x57f7) ; //movra	0x1f9 
  
  if (FMReadOne(Addr_Flash_OptionProtec+5)==1)//取option 2 高字节 确定是16M 8M 4M 2M 1M 455K哪一种
  {
    optH =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    return 0;
  }    
  //FAS=? FDA=? RCSMTB=?
  optH &= 0x3f;
  MC321_IRC_INST(0x3c00+optH) ; //movai	0xf8
  MC321_IRC_INST(0x57fe) ; //movra	0x1fe  
  
  if((optH&0x07)==0x00)//16M
  {
    tempH=0x7c;
  }
  else if((optH&0x07)==0x01)//8M
  {
    tempH=0x6c;
  }
  else if((optH&0x07)==0x02)//4M
  {
    tempH=0x5c;
  }
  else if((optH&0x07)==0x03)//2M
  {
    tempH=0x4c;   
  }
  else if((optH&0x07)==0x04)//1M
  {
    tempH=0x3c; 
  }
  else if((optH&0x07)==0x05)//455K
  {
    tempH=0x1c;   
  }
  else
  {
    return 0;
  }
  //FCPU=?
  MC321_IRC_INST(0x3c00+tempH) ; //movai	
  MC321_IRC_INST(0x57f9) ; //movra	0x1f9
  

  if (FMReadOne(Addr_Flash_OptionProtec+4)==1)//取option 2 低字节 
  {
    optL =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    return 0;
  }    
  //VDSEL=? TADJH=?
  optL &= 0x03;
  MC321_IRC_INST(0x3c00+optL) ; //movai	0xf8
  MC321_IRC_INST(0x57fd) ; //movra	0x1fe

  MC321_IRC_INST(0x3c01) ;  //movai	0x01 高频系统时钟由PSDO输出
  MC321_IRC_INST(0x57f8) ; 
  MC321_IRC_INST(0x0000) ;
    

  
  //VDSEL=? TADJH=?
  optL=TADJ;
  optL &= 0x03;
  MC321_IRC_INST(0x3c00+optL) ; //movai	0xf8
  MC321_IRC_INST(0x57fd) ; //movra	0x1fe
  
  optL=OPTION_FRT2;
  optH=OPTION_FRT;

  MC32P5222FT_11Bit(optL,optH);
            
  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    //OPTION_FRT=mm/256;   
    //OPTION_FRT2=mm%256;
    //OPTION_FRT=OPTION_FRT|(optH&0xf8);
    return 1;
  } 
  OPTION_FRT=0xff;   
  OPTION_FRT2=0xff;
  ERORR_VALUE=IRC_Value_false;
  return 0;
}



u8 MC32P5222_Program()
{
  u8  ReCnt=0,op_addr=0;
  u16  Fdata,ROMReadData;
  //u8 temp;
  u8	FdataL,FdataH,option_shift=0;
  u8  ReFlag=0;
  u16  CheckSum=0;
  
  u16 mcu_addr=0;
  
  u16   CRC_value=0xFFFF;
  u16   i,j,data[16];
  
  u8    id_length=0;
  u16   id_addr0,id_addr1,id_addr2,id_addr3;
  u32   id_end,current_id,max_id; 
  
  u8    ForceReCnt=0;  
  u8    SuccessCnt;   
  u16   Addres;   
  
  WriteConfig_xx.RomBit=0xffff;
  WriteConfig_xx.ProTime=1;
  WriteConfig_xx.ReadProAddrAndData=1;
  WriteConfig_xx.Page=0;
  WriteConfig_xx.Addr_Flash_OptionStart=Addr_Flash_OptionProtec;
  WriteConfig_xx.SecurityOption=0;
  WriteConfig_xx.SecurityBit=15;    
  WriteConfig_xx.WriteCheckNeed=0;
  WriteConfig_xx.ReadCnt=1;
  WriteConfig_xx.ReadOkCnt=1;
  WriteConfig_xx.ReWriteCnt=3;
  WriteConfig_xx.ForceWriteCnt=1;
  WriteConfig_xx.AllCheckNeed=2;  
  
//------------------------------先进模式----------------------------
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(5);
  //power on & model in
  MC32P5222_MODEL_IN(vdd30);
  CLN_VDD2GND_Off;
  VPP12V_On;
  VPP_IO_On;//FPGA VPP

  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID) 
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  } 

//------------------------------MTP烧写----------------------------   
  if(DeviceConfig_xx.MCU_ID==0x5222)
  {
    FMReadOne(0x012120+8); 
    if(!(Rxdata&0x01))//MTP 烧写
    {
  //    FM_CS_0;
  //    FMReadMore(Addr_Flash_ROMStart+0x8000-1);     
      if(Rxdata&0x10)//Byte Write
      {    
        //先写
        FM_CS_0;
        FMReadMore(Addr_Flash_ROMStart+0x8000-1);  
        for(i=0;i<1024;i++)
        {   
          SPI_ReadByte();
          FdataL=Rxdata;
          SPI_ReadByte();
          FdataH=Rxdata;
          MC32P5222_MTPWriteByte(0xc000+i,FdataL);
        }     
        FM_CS_1;
        
        //常压PROG MARGIN 2(3.3V)校验、常压ERASE MARGIN 2 (3.3V)
        FM_CS_0;
        FMReadMore(Addr_Flash_ROMStart+0x8000-1);  
        for(i=0;i<1024;i++)
        {   
          SPI_ReadByte();
          FdataL=Rxdata;
          SPI_ReadByte();
          FdataH=Rxdata;
          //ROMReadData=MC32P5222_MTPByteRead(0xc000+i);
          //武工、IP提供商建议只用ProMargin2_PDIN模式
//          ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc000+i,ProMargin1_PDIN,ProMargin_PTM);
//          if(ROMReadData!=FdataL)
//          {
//            ERORR_VALUE=MTP_Write_false;
//            return 0;
//          }    
          ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc000+i,ProMargin2_PDIN,ProMargin_PTM);
          if(ROMReadData!=FdataL)
          {
            ERORR_VALUE=MTP_Write_false;
            return 0;
          }          
//          ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc000+i,ProMargin3_PDIN,ProMargin_PTM);
//          if(ROMReadData!=FdataL)
//          {
//            ERORR_VALUE=MTP_Write_false;
//            return 0;
//          }                   
        }    
        FM_CS_1;
      } 
      else//Page Write
      {
        //擦chip
        MC32P5222_MTPChipErase(0xc000);  
        //常压ERASE MARGIN 2 (3.3V)校验
        for(i=0;i<1024;i++)
        {        
//          ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc000+i,EraMargin1_PDIN,EraMargin_PTM);
//          if(ROMReadData!=0xff)
//          {
//            ERORR_VALUE=MTP_Write_false;
//            return 0;
//          }  
          ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc000+i,EraMargin2_PDIN,EraMargin_PTM);
          if(ROMReadData!=0xff)
          {
            ERORR_VALUE=MTP_Write_false;
            return 0;
          }  
//          ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc000+i,EraMargin3_PDIN,EraMargin_PTM);
//          if(ROMReadData!=0xff)
//          {
//            ERORR_VALUE=MTP_Write_false;
//            return 0;
//          }           
        }    

        //写page
        FM_CS_0;
        FMReadMore(Addr_Flash_ROMStart+0x8000-1);   
        for(i=0;i<64;i++)
        {
          for(j=0;j<16;j++)
          {
            SPI_ReadByte();
            FdataL=Rxdata;
            SPI_ReadByte();
            FdataH=Rxdata;
            data[j]=FdataL;
            MC32P5222_MTPWritePage(0xc000+i*16,j,FdataL);          
          }
          for(j=0;j<16;j++)
          {
            ROMReadData=MC32P5222_MTPByteRead(0xc000+i*16+j);    
            if(ROMReadData!=data[j])
            {
              ERORR_VALUE=MTP_Write_false;
              return 0;
            }
          }     
        }
        FM_CS_1;
        
        //常压PROG MARGIN 2(3.3V)校验
        FM_CS_0;
        FMReadMore(Addr_Flash_ROMStart+0x8000-1);  
        for(i=0;i<1024;i++)
        {   
          SPI_ReadByte();
          FdataL=Rxdata;
          SPI_ReadByte();
          FdataH=Rxdata;
          //ROMReadData=MC32P5222_MTPByteRead(0xc000+i);
//          ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc000+i,ProMargin1_PDIN,ProMargin_PTM);
//          if(ROMReadData!=FdataL)
//          {
//            ERORR_VALUE=MTP_Write_false;
//            return 0;
//          }   
          ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc000+i,ProMargin2_PDIN,ProMargin_PTM);
          if(ROMReadData!=FdataL)
          {
            ERORR_VALUE=MTP_Write_false;
            return 0;
          }          
//          ROMReadData=MC32P5222_MTPByteRead_DiffMode(0xc000+i,ProMargin3_PDIN,ProMargin_PTM);
//          if(ROMReadData!=FdataL)
//          {
//            ERORR_VALUE=MTP_Write_false;
//            return 0;
//          }               
        }    
        FM_CS_1;
        
      }
      VDD30V_Off;
      POWER_OFF(vpp00,vdd00);
      return 1;  
    }
  }

//------------------------------IRC校准---------------------------- 
  if (DeviceConfig_xx.IRC_Select==0x01)//正式代码
  //if (DeviceConfig_xx.IRC_Select==0x00)//调试代码
  {
    ReFlag=MC32P5222IRC(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
    if(ReFlag==1)
    {
      ReFlag=MC32P5222IRC_Check(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
    }
    ConfigTIM4();
    if(ReFlag == 0) 
    {
      ERORR_VALUE=IRC_Value_false;
      return 0;
    }
  }      
  
//------------------------------再进模式----------------------------
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(5);
  //power on & model in
  if(DeviceConfig_xx.MCU_ID==0x5222)
  {
    MC32P5222_MODEL_IN(vdd30);
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
 
//------------------------------Write ROM---------------------------- 
  if (DeviceConfig_xx.WriteEn & 0x0f)
  {
    CheckSum=0;
    FM_CS_0;
    FMReadMore(Addr_Flash_ROMStart+DeviceConfig_xx.RomFirAddr-1);
    Fdata =Rxdata;

    //MCU的芯片才可能会有滚动码这个需求    其他芯片不需要关于iic_data的代码
    id_length=iic_data[1];
    id_addr0=(iic_data[2]<<8)+iic_data[3];// ADDRL
    id_addr1=(iic_data[4]<<8)+iic_data[5];
    id_addr2=(iic_data[6]<<8)+iic_data[7];
    id_addr3=(iic_data[8]<<8)+iic_data[9];//ADDRH
    current_id=(iic_data[10]<<24)+(iic_data[11]<<16)+(iic_data[12]<<8)+iic_data[13];//当前已经烧写过的芯片写入的编号
    id_end=(iic_data[15]<<24)+(iic_data[16]<<16)+(iic_data[17]<<8)+iic_data[18];//烧写成功的次数 限制值
    max_id=(iic_data[27]<<24)+(iic_data[28]<<16)+(iic_data[29]<<8)+iic_data[30]; //max-id//芯片最大编号
    if (current_id>max_id)
    {
      iic_data[10]=iic_data[23];
      iic_data[11]=iic_data[24];
      iic_data[12]=iic_data[25];
      iic_data[13]=iic_data[26];
    }
    if (iic_data[0]==0x0a)//烧写次数限制功能打开
    {
      if (OKcounter>=id_end)//烧写成功的次数达到限制值
      {
        ERORR_VALUE=OK_Counter_Full;
        return 0; // full limit
      }
    }
    
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
 
      
      //Fdata=Fdata & 0x3fff;
      //2012.09.12 dynamic id
      mcu_addr=OTP_ADDR/2;
      OTP_ADDR=OTP_ADDR+2;						
      if (iic_data[0]==0x0a) //id enable MCU的芯片才可能会有滚动码这个需求
      {
        if (mcu_addr==id_addr0)
        {
          //Fdata=iic_data[13];
          Fdata=FdataH*0x100+iic_data[13]; // must keep the H byte for code
        }
        else if ((mcu_addr==id_addr1)&& (id_length>1))
        {
          //Fdata=iic_data[12];
          Fdata=FdataH*0x100+iic_data[12];
        }
        else if ((mcu_addr==id_addr2)&& (id_length>2))
        {
          //Fdata=iic_data[11];
          Fdata=FdataH*0x100+iic_data[11];
        }
        else if ((mcu_addr==id_addr3)&&(id_length>3))
        {
          //Fdata=iic_data[10];
          Fdata=FdataH*0x100+iic_data[10];
        }
      }
      //计算CRC
      FdataL=Fdata%256;
      CRC_value=CountCRC(FdataH,FdataL,CRC_value);//     
      //------------------------------------------------
      Rxdata=Fdata;
      
      ReCnt=WriteConfig_xx.ReWriteCnt;
      while(ReCnt--)  //start wr rom
      {	 
        ForceReCnt=WriteConfig_xx.ForceWriteCnt;
        if (Fdata!=0xffff)
        {
          while(ForceReCnt--)
          {
            MC321_W(Fdata); //wr data to rom
          }
        }
        SuccessCnt = 0;
        if(WriteConfig_xx.WriteCheckNeed)
        {
          for(i=0;i<WriteConfig_xx.ReadCnt;i++)
          {
            ROMReadData=MC321_R();
            if (ROMReadData==Fdata)
            {
              SuccessCnt++;
            }
            if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
            {
              break;
            }
          } 
        }
        else
        {
          SuccessCnt = WriteConfig_xx.ReadOkCnt;
        }
        
        if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//在规定读取校验次数下达到所要求的成功次数 烧写成功
        {
          //ReCnt=0;
          break;
        }
        else
        {
          ERORR_VALUE=ROM_Write_false;
        }
        
        if (ReCnt==0)
        {
          OTP_ADDR= mcu_addr;
          return 0;
        }                    
      }
      if (OTP_ADDR<DeviceConfig_xx.RomEndAddr)
      {
        MC301_Instruct(AddrINC);
        if (DeviceConfig_xx.MCU_ID==0x5222)
        {
          mcu_addr = MC321_PreRead(PAddrRe);
          if (mcu_addr!=OTP_ADDR/2)
          {
            ERORR_VALUE=ADDR_EER;
            return 0;
          }
        }
      }      
    }
    FM_CS_1;
  }  
 
//------------------------------Write OPTION ---------------------------- 
  if(DeviceConfig_xx.WriteEn & 0xf0)
  {
    FM_CS_0;
    FMReadMore(Addr_Flash_OptionProtec-1);
    Fdata =Rxdata;
    
    for(op_addr=0;op_addr<DeviceConfig_xx.OptionSize;op_addr++)//
    {
      SPI_ReadByte();//从flash中取一个字节
      FdataL=Rxdata;
      //CheckSum +=FdataL;
      SPI_ReadByte();//再从flash中取一个字节
      FdataH=Rxdata;
      //CheckSum +=FdataH;
      Fdata=FdataH*0x100+FdataL;//组成一个字
      
      //option0 
      if(op_addr==0)
      {          
        Fdata|=0x8000;//不写加密位
      }      
      else if(op_addr==1)
      {
        ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);
        if((ROMReadData&0x3ff)==0x3ff)//没写过频率校准值
        {
          Fdata = (OPTION_FRT<<8)|OPTION_FRT2;
        }
        else
        {
          Fdata = ROMReadData;
        }   
      }
      else if(op_addr==3)//
      {          
        ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);
        if(ROMReadData==0xffff)//没写过LVD调校位
        {
          ROMReadData = MC321_ReadByte(0x800C);
          if(ROMReadData==0xffff)//没有成测值
          {
            ROMReadData = MC321_ReadByte(0x800B);
            if(ROMReadData==0xffff)//没有中测值
            {
              ERORR_VALUE=OPTION_Write_false;//没写过adc reference频率校准值 也没有成测adc reference值 也没有中测adc reference值 报错
              return 0;
            }
          }  
          else
          {
            ROMReadData>>=8;
            ROMReadData|=0xff00;
          }
        }
        Fdata=ROMReadData;
      }      
   
      WriteConfig_xx.OptionValue[op_addr] = Fdata;//烧option之前先把值存起来，以方便后续全面校验时读值
      WriteConfig_xx.OptionAddr[op_addr]=DeviceConfig_xx.OptionAddr+op_addr+option_shift;
      ReCnt=WriteConfig_xx.ReWriteCnt;
      while(ReCnt--)  
      {	
        ForceReCnt=WriteConfig_xx.ForceWriteCnt;
        if (Fdata != 0xffff)  // 
        {
          while(ForceReCnt--)
          {
            MC321_WriteByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift,Fdata);
          }
        }        
	SuccessCnt = 0;
	if (WriteConfig_xx.WriteCheckNeed)
	{
	  for(i=0;i<WriteConfig_xx.ReadCnt;i++)
	  {
	    ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);         
	    if (ROMReadData==Fdata)
	    {
	      SuccessCnt++;
	    }
	    else
	    {
	    }
	    if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
	    {
	      break;
	    }
	  }
	}
	else
	{
	  SuccessCnt = WriteConfig_xx.ReadOkCnt;
	}
        if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//在规定读取校验次数下达到所要求的成功次数 烧写成功
        {
          //ReCnt=0;
          break;
        }
	else
	{
	  ERORR_VALUE=OPTION_NOMALL_ERROR;
	}
	
        if (ReCnt==0)
        {        
          OTP_ADDR=DeviceConfig_xx.OptionAddr+op_addr+option_shift;
          return 0;
        }                     
      }
    }
    FM_CS_1;
  }  
  
//------------------------------write checksum----------------------------      
  ReCnt=0;  
  while(++ReCnt<3)
  {
    if(DeviceConfig_xx.MCU_ID==0x5222)
    {
      MC321_WriteByte(0x800f,CheckSum);
    }
  }
  
//------------------------------频率验证 取写入芯片的温度校准值和频率校准值----------------------------  
  if (DeviceConfig_xx.IRC_Select==0x01)//正式代码
  //if (DeviceConfig_xx.IRC_Select==0x00)//调试代码
  {
    ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+1+option_shift); //set ADDR
    OPTION_FRT=ROMReadData/256;
    OPTION_FRT2=ROMReadData%256;
    ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+2+option_shift); //set ADDR
    TADJ=ROMReadData%256;
    ReFlag=MC32P5222IRC_ModeInCheck(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
    ConfigTIM4();//关闭Timer3、Timer4
    if(ReFlag == 0) 
    {
      ERORR_VALUE=IRC_Value_false;
      return 0;
    }
  }   
 
//------------------------------再进模式----------------------------
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(5);
  //power on & model in
  if(DeviceConfig_xx.MCU_ID==0x5222)
  {
    MC32P5222_MODEL_IN(vdd30);
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
//------------------------------ROM CRC----------------------------   
  if(DeviceConfig_xx.MCU_ID==0x5222)
  {
    for(i=0xFF0;i<=0xFFF;i++)//0xFF0-0xFFF CRC按0x0000计算
    {
      CRC_value=CountCRC(0x00,0x00,CRC_value);//
    }      
    mcu_addr=0xfff;
  }
  ROMReadData = GetCheckSum(mcu_addr+1);
  if(ROMReadData!=CRC_value)
  {
    ERORR_VALUE=CHECK_PROGRAM_ERROR;
    OTP_ADDR= mcu_addr;
    return 0;    
  }

//------------------------------rom 全面读校验----------------------------   
  if (DeviceConfig_xx.WriteEn & 0x0f)
  {
    if (WriteConfig_xx.AllCheckNeed > 0)
    {
      FM_CS_0;
      FMReadMore(Addr_Flash_ROMStart+DeviceConfig_xx.RomFirAddr-1);
      Fdata =Rxdata;
      
      //MCU的芯片才可能会有滚动码这个需求    其他芯片不需要关于iic_data的代码
      id_length=iic_data[1];
      id_addr0=(iic_data[2]<<8)+iic_data[3];// ADDRL
      id_addr1=(iic_data[4]<<8)+iic_data[5];
      id_addr2=(iic_data[6]<<8)+iic_data[7];
      id_addr3=(iic_data[8]<<8)+iic_data[9];//ADDRH
      current_id=(iic_data[10]<<24)+(iic_data[11]<<16)+(iic_data[12]<<8)+iic_data[13];//当前已经烧写过的芯片写入的编号
      id_end=(iic_data[15]<<24)+(iic_data[16]<<16)+(iic_data[17]<<8)+iic_data[18];//烧写成功的次数 限制值
      max_id=(iic_data[27]<<24)+(iic_data[28]<<16)+(iic_data[29]<<8)+iic_data[30]; //max-id//芯片最大编号
      if (current_id>max_id)
      {
        iic_data[10]=iic_data[23];
        iic_data[11]=iic_data[24];
        iic_data[12]=iic_data[25];
        iic_data[13]=iic_data[26];
      }
      if (iic_data[0]==0x0a)//烧写次数限制功能打开
      {
        if (OKcounter>=id_end)//烧写成功的次数达到限制值
        {
          ERORR_VALUE=OK_Counter_Full;
          return 0; // full limit
        }
      }
      
      ROMReadData=MC321_ReadByte(0x0000); //set ADDR
      for(OTP_ADDR=DeviceConfig_xx.RomFirAddr;OTP_ADDR<=DeviceConfig_xx.RomEndAddr;)
      {
	SPI_ReadByte();
	FdataL=Rxdata;
	SPI_ReadByte();
	FdataH=Rxdata;
	Fdata=FdataH*0x100+FdataL;
	
	mcu_addr=OTP_ADDR/2;
	OTP_ADDR=OTP_ADDR+2;
 
        if (iic_data[0]==0x0a) //id enable MCU的芯片才可能会有滚动码这个需求
        {
          if (mcu_addr==id_addr0)
          {
            //Fdata=iic_data[13];
            Fdata=FdataH*0x100+iic_data[13]; // must keep the H byte for code
          }
          else if ((mcu_addr==id_addr1)&& (id_length>1))
          {
            //Fdata=iic_data[12];
            Fdata=FdataH*0x100+iic_data[12];
          }
          else if ((mcu_addr==id_addr2)&& (id_length>2))
          {
            //Fdata=iic_data[11];
            Fdata=FdataH*0x100+iic_data[11];
          }
          else if ((mcu_addr==id_addr3)&&(id_length>3))
          {
            //Fdata=iic_data[10];
            Fdata=FdataH*0x100+iic_data[10];
          }
        }        
	
	if (WriteConfig_xx.AllCheckNeed == 2)
	{
          for(i=1;i<3;i++)
          {
            SuccessCnt = 0;
            for(j=0;j<WriteConfig_xx.ReadCnt;j++)
            {
              ROMReadData=MC321_R_DiffMode(i);
              if (ROMReadData==Fdata)
              {
                SuccessCnt++;
              }
              else
              {
              }
              if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
              {
                break;
              }
            } 
            
            if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//在规定读取校验次数下达到所要求的成功次数 烧写成功
            {
            }
            else
            {
              if(i==1)
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
          if (i==3)//3种模式读取都和写入的数据一致 烧写成功 2
          {
            //break;
          }
          else
          {
            OTP_ADDR= mcu_addr;
            return 0;
          }
	}
	else
        {
          SuccessCnt = 0;
          for(j=0;j<WriteConfig_xx.ReadCnt;j++)
          {
            ROMReadData=MC321_R();
            if (ROMReadData==Fdata)
            {
              SuccessCnt++;
            }
            else
            {
            }
            if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
            {
              break;
            }
          } 
          
          if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//在规定读取校验次数下达到所要求的成功次数 烧写成功
          {
            //break;
          }
          else
          {
            ERORR_VALUE=NOMALL_ERROR;
            OTP_ADDR= mcu_addr;
            return 0;
          }
        }
	
	if (OTP_ADDR<DeviceConfig_xx.RomEndAddr)
	{
	  MC301_Instruct(AddrINC);
	}
      }
      FM_CS_1;
    }
  }
  
//------------------------------option全面读校验----------------------------    
  if(DeviceConfig_xx.WriteEn & 0xf0)
  {
    if(WriteConfig_xx.AllCheckNeed > 0)
    {
      for(op_addr=0;op_addr<DeviceConfig_xx.OptionSize;op_addr++)
      {
	Fdata=WriteConfig_xx.OptionValue[op_addr];
        Addres=WriteConfig_xx.OptionAddr[op_addr];

	if (WriteConfig_xx.AllCheckNeed == 2)
	{
          for(i=1;i<3;i++)
          {
            SuccessCnt = 0;
            for(j=0;j<WriteConfig_xx.ReadCnt;j++)
            {
              ROMReadData=MC321_ReadByte_DiffMode(i,Addres);
              if (ROMReadData==Fdata)
              {
                SuccessCnt++;
              }
              else
              {
              }
              if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
              {
                break;
              }
            } 
            if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//在规定读取校验次数下达到所要求的成功次数 烧写成功
            {
            }
            else
            {
              if(i==1)
              {
                ERORR_VALUE=OPTION_MAGRIN1_ERROR;
              }
              else if(i==2)//测试用
              {
                ERORR_VALUE=OPTION_OFF_MAGRIN_ERROR;
              }
              break;
            }
          }
          if (i==3)//3种模式读取都和写入的数据一致 烧写成功 2
          {
            
          }
          else
          {
            OTP_ADDR= Addres;
            return 0;
          } 
	}
	else
        {
          SuccessCnt = 0;
	  for(j=0;j<WriteConfig_xx.ReadCnt;j++)
	  {
	    ROMReadData=MC321_ReadByte(Addres);
	    if (ROMReadData==Fdata)
	    {
	      SuccessCnt++;
	    }
	    else
	    {
	    }
	    if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
	    {
	      break;
	    }
	  } 
	  if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//在规定读取校验次数下达到所要求的成功次数 烧写成功
	  {
            //break;
	  }
	  else
	  {
	    ERORR_VALUE=OPTION_NOMALL_ERROR;
	    OTP_ADDR= Addres;
            return 0;
	  }
        }
      }
    }
  }
    
//------------------------------write protect bit----------------------------    
  if(DeviceConfig_xx.WriteEn & 0xf0)
  {  
    FM_CS_0;
    FMReadMore(Addr_Flash_Option-1);
    Fdata =Rxdata;
    SPI_ReadByte();//从flash中取一个字节
    FdataL=Rxdata;
    SPI_ReadByte();//再从flash中取一个字节
    FdataH=Rxdata;
    Fdata=FdataH*0x100+FdataL;//组成一个字 

    ReCnt=WriteConfig_xx.ReWriteCnt;
    while(ReCnt--) 
    {	
      ForceReCnt=WriteConfig_xx.ForceWriteCnt;      
      if (Fdata != 0xffff)  // 
      {
        while(ForceReCnt--)
        {
          MC321_WriteByte(DeviceConfig_xx.OptionAddr+option_shift,Fdata);
        } 
      } 
      if (WriteConfig_xx.AllCheckNeed == 2)
      {
        for(i=1;i<3;i++)
        {
          SuccessCnt = 0;
          for(j=0;j<WriteConfig_xx.ReadCnt;j++)
          {
            ROMReadData=MC321_ReadByte_DiffMode(i,DeviceConfig_xx.OptionAddr+option_shift);
            if (ROMReadData==Fdata)
            {
              SuccessCnt++;
            }
            else
            {
            }
            if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
            {
              break;
            }
          } 
          if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//在规定读取校验次数下达到所要求的成功次数 烧写成功
          {
          }
          else
          {
            if(i==1)
            {
              ERORR_VALUE=OPTION_MAGRIN1_ERROR;
            }
            else if(i==2)//测试用
            {
              ERORR_VALUE=OPTION_OFF_MAGRIN_ERROR;
            }
            break;
          }
        }
        if (i==3)//3种模式读取都和写入的数据一致 烧写成功 2
        {
          break;
        }
        else
        {
          OTP_ADDR= DeviceConfig_xx.OptionAddr+option_shift;
          return 0;
        } 
      }  
      else if(WriteConfig_xx.AllCheckNeed == 1)
      {
        SuccessCnt = 0;
        for(j=0;j<WriteConfig_xx.ReadCnt;j++)
        {
          ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+option_shift);
          if (ROMReadData==Fdata)
          {
            SuccessCnt++;
          }
          else
          {
          }
          if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
          {
            break;
          }
        } 
        if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//在规定读取校验次数下达到所要求的成功次数 烧写成功
        {
          break;//退出while
        }
        else
        {
          ERORR_VALUE=OPTION_NOMALL_ERROR;
          OTP_ADDR= DeviceConfig_xx.OptionAddr+option_shift;
          return 0;
        }
      }
    }
  }  
  FM_CS_1; 
  
  
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  return 1;  
}