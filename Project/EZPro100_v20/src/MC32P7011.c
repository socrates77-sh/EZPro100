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
#include "MC32P7011.h"
#include "MC9033.h"
void MC32P7011_MODEL_IN(u8 VddFlag)
{
	u8 temp;
	//power on & model in
	POWER_OFF(vpp00,vdd00);
	Delay_1ms(100);
	OTP_SCK_0;
	OTP_SDI_0; 

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
//        VDD_IO_Off; //VDD IO PIN SIGNAL

	NOP;
        NOP;
        NOP
//        VDD_IO_On;

	CLN_VDD2GND_Off;
	VPP12V_On;
//        VPP_IO_On;
        
        Delay_1ms(1);//李秀峰加  要求延时时间长一点 需要时间去加载0x8000值到0xc000地址
        
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
//        VPP_IO_Off;
	CLN_VDD2GND_On;
	Delay_100us(5);
        NOP;

	OTP_SDI_1;
	NOP;
	OTP_SDI_0;
	NOP;

	CLN_VDD2GND_Off;
	VPP12V_On;
//        VPP_IO_On;
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
//        VPP_IO_Off;
	CLN_VDD2GND_On;
	Delay_100us(5);
        NOP;
        
	OTP_SDI_1;
	NOP;
	OTP_SDI_0;
	Delay_1ms(1);
        NOP;

}
//IRC校验测试版
void MC32P7011FTtest_10Bit(u16 irc_adj_value)
{
  u16 temp;
  u8  adjH,adjL;
  u16 i;
  u16 cnt[3];
  //OPTION 1：
  //bit9:0  --  bit9:0   OSCCAL
  //bit15:10  --  bit6:0   TADJ  
  adjH=irc_adj_value/256; //H
  adjL=irc_adj_value%256; //L  
  
  temp=0x2d00+adjL;
  MC321_IRC_INST(temp);
  MC321_IRC_INST(0x1f8a); //option1 L
  MC321_IRC_INST(0x0000);

  if (FMReadOne(Addr_Flash_OptionProtec+5)==1)//取flash中 option1 高字节 TADJ
  {
    //optH =Rxdata;
  }
  /*
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }  
  */
  adjH = (adjH&0x03)|(Rxdata&0xfc);//TADJ不能改变
  temp=0x2d00+adjH;
  MC321_IRC_INST(temp); //
  MC321_IRC_INST(0x1f8b); //option1 H  
  
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

//IRC 校准 正式版
void MC32P7011FTtest_10Bit_z(u16 irc_adj_value,u8 tadj)
{
  u16 temp;
  u8  adjH,adjL;
  u16 i;
  u16 cnt[3];
  //OPTION 1：
  //bit9:0  --  bit9:0   OSCCAL
  //bit15:10  --  bit6:0   TADJ  
  adjH=irc_adj_value/256; //H
  adjL=irc_adj_value%256; //L  
  
  temp=0x2d00+adjL;
  MC321_IRC_INST(temp);
  MC321_IRC_INST(0x1f8a); //option1 L
  MC321_IRC_INST(0x0000);

  tadj<<=2;
  adjH = (adjH&0x03)|(tadj&0xfc);//TADJ不能改变
  temp=0x2d00+adjH;
  MC321_IRC_INST(temp); //
  MC321_IRC_INST(0x1f8b); //option1 H  
  
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

//IRC校验测试版
u8 MC32P7011IRC_10Bit(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i;
  u8 optL,optH;
  u16 mm,Fdata,temp0_FT,temp1_FT;
  u8 OPTION_FAS;
  
  FM_CS_0;
  mm=0x400 ;//& (rdv<<8);
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(10);
  //power on & model in
  MC32P7011_MODEL_IN(vdd30);

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

/* 
  MC321_IRC_INST(0x2d00);
  MC321_IRC_INST(0x1f89);     //写opt2 H  选择内部RC   
  
  MC321_IRC_INST(0x2d01);     //movai 0x01
  MC321_IRC_INST(0x1f80);     //movra 0x80 //set HIRC output to SDO

  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;   
*/ 
  
  if (FMReadOne(Addr_Flash_OptionProtec+5)==1)//取flash中 option1 高字节
  {
    optH =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }    
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
    optL = 0x70;//opt2 L 分频系数128
  }
  else if(OPTION_FAS == 0x02)//8M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1239;
    IRC_FreqMax = 1261;   
    optL = 0x60;//opt2 L 分频系数64
  }
  else if(OPTION_FAS == 0x04)//4M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1239;
    IRC_FreqMax = 1261;     
    optL = 0x50;//opt2 L 分频系数32
  }
  else if(OPTION_FAS == 0x06)//2M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1239;
    IRC_FreqMax = 1261;    
    optL = 0x40;//opt2 L 分频系数16
  }   
  else if(OPTION_FAS == 0x08)//1M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1239;
    IRC_FreqMax = 1261;    
    optL = 0x30;//opt2 L 分频系数8
  }   
  else if(OPTION_FAS == 0x0A)//455K
  {
    IRC_FreqType = 2275;
    IRC_FreqMin = 2255;
    IRC_FreqMax = 2295;    
    optL = 0x10;//opt2 L 分频系数2
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

  
  mm=0x200;//频率校准位先给中间值
  OPTION_FRT=0; //initial 
  OPTION_FRT2=0; 
  for(i=0;i<10;i++)
  {
    MC32P7011FTtest_10Bit(mm);
    if (IRC_VALUE == IRC_FreqType)
    {
      break;
    }
    if (IRC_VALUE<IRC_FreqType)
    {
      mm |=0x200 >>i;
    } 
    else
    {
      mm &=~(0x200>>i);
    }
    mm |=0x100>>i;
  }
  MC32P7011FTtest_10Bit(mm);
  temp1_FT=IRC_VALUE;
  MC32P7011FTtest_10Bit(mm+1);
  temp0_FT=IRC_VALUE;
  if((temp0_FT-IRC_FreqType)<(IRC_FreqType-temp1_FT))
  {
    mm=mm+1;
    IRC_VALUE=temp0_FT;
  }
  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    OPTION_FRT=mm/256;
    OPTION_FRT2=mm%256;
    return 1;//校验成功，结束校验
  }

  OPTION_FRT=0xff;
  OPTION_FRT2=0xff;
  return 0;
  //}校验结束
}
u8 MC32P7011_ChangeTempIRC(u8 TADJ_temp,u16 IRC_FreqType,u16 IRC_FreqMin,u16 IRC_FreqMax)
{
  u8 i;
  u16 temp0_FT,temp1_FT;
  u16 OSCCAL_temp=0x200;//频率校准位先给中间值
  OPTION_FRT=0xff; //initial 
  OPTION_FRT2=0xff; 
  for(i=0;i<10;i++)
  {
    MC32P7011FTtest_10Bit_z(OSCCAL_temp,TADJ_temp);
    if (IRC_VALUE == IRC_FreqType)
    {
      break;
    }
    if (IRC_VALUE<IRC_FreqType)
    {
      OSCCAL_temp |=0x200 >>i;
    } 
    else
    {
      OSCCAL_temp &=~(0x200>>i);
    }
    OSCCAL_temp |=0x100>>i;
  }
  MC32P7011FTtest_10Bit_z(OSCCAL_temp,TADJ_temp);
  temp1_FT=IRC_VALUE;
  MC32P7011FTtest_10Bit_z(OSCCAL_temp+1,TADJ_temp);
  temp0_FT=IRC_VALUE;
  if((temp0_FT-IRC_FreqType)<(IRC_FreqType-temp1_FT))
  {
    OSCCAL_temp=OSCCAL_temp+1;
    IRC_VALUE=temp0_FT;
  }
  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    OPTION_FRT=OSCCAL_temp/256;
    OPTION_FRT2=OSCCAL_temp%256;
    OPTION_FRT=(TADJ_temp<<2)|OPTION_FRT;
    return 1;//当前温度值条件下校验成功，结束校验
  }
  return 0;
}

//IRC校准正式版
u8 MC32P7011IRC_10Bit_z(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i,j;
  u8 optL,optH;
  u16 mm,Fdata;
  u8 OPTION_FAS;
  u8 OPTION_TEMPADJ_1,OPTION_TEMPADJ_2,OPTION_TEMPADJ_3;
  
  FM_CS_0;
  mm=0x400 ;//& (rdv<<8);
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(10);
  //power on & model in
  MC32P7011_MODEL_IN(vdd30);

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
  
//  if (FMReadOne(Addr_Flash_OptionProtec+5)==1)//取flash中 option1 高字节
//  {
//    optH =Rxdata;
//  }
//  else
//  {
//    ERORR_VALUE=FM_Read_false;
//    FM_CS_1;
//    return 0;
//  }    
//  mm=0x2d00+optH;
//  MC321_IRC_INST(mm);
//  MC321_IRC_INST(0x1f8b);     //写opt1 H  写温度调校位
  
  if (FMReadOne(Addr_Flash_OptionProtec+6)==1)//取flash中 option2 低字节 FAS
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
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x1f8c);     //写opt2 L  FAS
  
  if (FMReadOne(Addr_Flash_OptionProtec+3)==1)//取flash中 option0 高字节 FOSCS
  {
    optH =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }    
  mm=0x2d00+optH;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x1f89);     //写opt2 H  FOSCS
  
  OPTION_FAS = optL & 0x0E; //确定是16M 8M 4M 2M 1M 455K哪一种

  if(OPTION_FAS == 0x00)//16M
  {
//    IRC_FreqType = 2500;
//    IRC_FreqMin = 2480;
//    IRC_FreqMax = 2520;  
    //optL = 0x70;//opt2 L 分频系数128
    optL = 0x60;//opt2 L 分频系数64
    OPTION_TEMPADJ_1=0x0a;
    OPTION_TEMPADJ_2=0x09;
    OPTION_TEMPADJ_3=0x0b;
  }
  else if(OPTION_FAS == 0x02)//8M
  {
//    IRC_FreqType = 1250;
//    IRC_FreqMin = 1240;
//    IRC_FreqMax = 1260;   
    optL = 0x60;//opt2 L 分频系数64
    OPTION_TEMPADJ_1=0x12;
    OPTION_TEMPADJ_2=0x11;
    OPTION_TEMPADJ_3=0x13;    
  }
  else if(OPTION_FAS == 0x04)//4M
  {
//    IRC_FreqType = 1250;
//    IRC_FreqMin = 1240;
//    IRC_FreqMax = 1260;     
    optL = 0x50;//opt2 L 分频系数32
    OPTION_TEMPADJ_1=0x18;
    OPTION_TEMPADJ_2=0x17;
    OPTION_TEMPADJ_3=0x19;    
  }
  else if(OPTION_FAS == 0x06)//2M
  {
//    IRC_FreqType = 1250;
//    IRC_FreqMin = 1240;
//    IRC_FreqMax = 1260;  
    optL = 0x40;//opt2 L 分频系数16
    OPTION_TEMPADJ_1=0x2c;
    OPTION_TEMPADJ_2=0x2b;
    OPTION_TEMPADJ_3=0x2d;    
  }   
  else if(OPTION_FAS == 0x08)//1M
  {
//    IRC_FreqType = 1250;
//    IRC_FreqMin = 1240;
//    IRC_FreqMax = 1260;  
    optL = 0x30;//opt2 L 分频系数8
    OPTION_TEMPADJ_1=0x16;
    OPTION_TEMPADJ_2=0x15;
    OPTION_TEMPADJ_3=0x17;    
  }   
  else if(OPTION_FAS == 0x0A)//455K
  {
//    IRC_FreqType = 2275;
//    IRC_FreqMin = 2257;
//    IRC_FreqMax = 2293;    
    optL = 0x10;//opt2 L 分频系数2
    OPTION_TEMPADJ_1=0x19;
    OPTION_TEMPADJ_2=0x18;
    OPTION_TEMPADJ_3=0x1A;  
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
  
  //先用三个温度值校
  for(j=0;j<3;j++)
  {
    if(j==0)
    {
      OPTION_TEMPADJ = OPTION_TEMPADJ_1;          
    }
    else if(j==1)
    {
      OPTION_TEMPADJ = OPTION_TEMPADJ_2;
    }
    else if(j==2)
    {
      OPTION_TEMPADJ = OPTION_TEMPADJ_3;
    }
    if(MC32P7011_ChangeTempIRC(OPTION_TEMPADJ,IRC_FreqType,IRC_FreqMin,IRC_FreqMax))
    {
      return 1;
    }
  }   
  
  //对于1M、455K 如果三个温度值校准都失败 再继续校
  if(OPTION_FAS==0x08)//1M
  {
    OPTION_TEMPADJ= 0x14;
  }
  else if(OPTION_FAS==0x0A)//455K
  {
    OPTION_TEMPADJ= 0x17;
  }
  else
  {
    OPTION_TEMPADJ= 0x00;
  }
  while(OPTION_TEMPADJ_3)
  {
    if(MC32P7011_ChangeTempIRC(OPTION_TEMPADJ,IRC_FreqType,IRC_FreqMin,IRC_FreqMax))
    {
      return 1;
    }
    OPTION_TEMPADJ_3--;
  }
  
  OPTION_FRT=0xff;
  OPTION_FRT2=0xff;
  return 0;
  //}校验结束
}


u16 GetCheckSum_7011(u16 check_num)
{
  u16 a;
  //SetOption(0xFFF2,0xFEFF);        //1. 设置OPT2 选择内部高频HIRC振荡器
  
  SetAddress(check_num);           //2．通过0x15命令设置校验个数
  //InputData(0xFFFF);               //3．通过0x16命令输入校验初始值 
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
u8 MC32P7011_Program()
{
  u8  i,ReCnt=0,op_addr=0;
  u16  Fdata,ROMReadData;
  u8	FdataL,FdataH,option_shift=0;
  u8  ReFlag=0;
  u16  CheckSum=0,CheckSum_c=0;
  u16 mcu_addr;
  
//先读MCU ID
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(10);
  //power on & model in
  MC32P7011_MODEL_IN(vdd30);
  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID) 
  {
          ERORR_VALUE=OTP_ModeIn_false;
          return 0;
  }  
  CLN_VDD2GND_Off;
  VPP12V_On;      

//再写page
  if (FMReadOne(Addr_Flash_OptionProtec)==1)
  {
      FdataL = Rxdata;
  }
  else
  {
      ERORR_VALUE=FM_Read_false;
      return 0;
  } 
  if (FMReadOne(Addr_Flash_OptionProtec+1)==1)
  {
      FdataH = Rxdata;
  }
  else
  {
      ERORR_VALUE=FM_Read_false;
      return 0;
  } 
  Fdata = FdataH*256+FdataL;
  if((FdataL&0x07)==0x03)//1k模式 第一次烧录
  {
    option_shift = 4;
    DeviceConfig_xx.RomEndAddr = 0x7ff;
  }
  else
  {
    if((FdataL&0x07)==0x02)//1k模式 第二次烧录
    {
      option_shift = 0;
      DeviceConfig_xx.RomEndAddr = 0x7ff;
    }
    else//2k模式 烧录
    {
      option_shift = 0;
      DeviceConfig_xx.RomEndAddr = 0xfff;
    }
  }
  Fdata &= 0xfff7;
  Fdata |= 0x0010;
  MC321_WriteByte(0x8000,Fdata);//
  ROMReadData=MC321_ReadByte(0x8000);
   if (ROMReadData==Fdata)
  {
  }
  else
  {
      ERORR_VALUE=ROM_Write_false;
      return 0;
  }  
 
//写完Page再进模式 写FOSCS 
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(10);
  //power on & model in
  MC32P7011_MODEL_IN(vdd30);

  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID) 
  {
          ERORR_VALUE=OTP_ModeIn_false;
          return 0;
  }  
  
  CLN_VDD2GND_Off;
  VPP12V_On;      

  //IRC adjust
  //因为A010目前有个bug，一旦芯片的同一个page烧了外部32K晶振，再次烧同一个page进测试模式后送指令无效
  //所以对上位机的低频时钟选择位进行判断，只要不选择内部RC，就不进行IRC校准
  //对于有这种bug的芯片，如果芯片的同一个page烧录已经用了外部32K晶振，想再次用内部RC，校准不过，是正常的
  if (FMReadOne(Addr_Flash_OptionProtec+3)==1)//opt0 H
  {
      FdataH = Rxdata;
  }
  else
  {
      ERORR_VALUE=FM_Read_false;
      return 0;
  } 
  if (FMReadOne(Addr_Flash_OptionProtec+2)==1)//opt0 L
  {
      FdataL = Rxdata;
  }
  else
  {
      ERORR_VALUE=FM_Read_false;
      return 0;
  } 
  if(FdataH&0x04)//不选择内部RC 就不校准
  {
    DeviceConfig_xx.IRC_Select=0;//不校准
    OPTION_FRT=0xff;
    OPTION_FRT2=0xff;
  }
  
  Fdata = FdataH*256+FdataL;
  Fdata|=0x8000;//不能写加密位
  MC321_WriteByte(DeviceConfig_xx.OptionAddr+option_shift,Fdata);////必须写FOSCS 
  ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+option_shift);
   if (ROMReadData==Fdata)
  {
  }
  else
  {
      ERORR_VALUE=ROM_Write_false;
      return 0;
  }  
   
//IRC校准  
  if (DeviceConfig_xx.IRC_Select==0x01)//正式代码
  //if (DeviceConfig_xx.IRC_Select==0x00)//调试代码
  {
    ReFlag=MC32P7011IRC_10Bit_z(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
//    ReFlag=MC32P7011IRC_10Bit(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
    ConfigTIM4();
    if(ReFlag == 0) 
    {
      ERORR_VALUE=IRC_Value_false;
      return 0;
    }
  }    

//再进模式 写ROM
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(10);
  //power on & model in
  if ((DeviceConfig_xx.MCU_ID==0x7011))
  {
    MC32P7011_MODEL_IN(vdd30);
  }
  else
  {
    return 0;
  }

  CLN_VDD2GND_Off;
  VPP12V_On;
//  VPP_IO_On;

  Fdata=MC321_ReadByte(0XFFFF);
  
  if (Fdata !=DeviceConfig_xx.MCU_ID) 
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  }    
  
  /*
  Fdata=MC321_ReadByte(0XC000);
  Fdata=MC321_ReadByte(0XC001);
  Fdata=MC321_ReadByte(0XC002);
  Fdata=MC321_ReadByte(0XC003);
  Fdata=MC321_ReadByte(0XC004);
  */
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
      CheckSum_c += FdataL;
      SPI_ReadByte();
      FdataH=Rxdata;
      CheckSum +=FdataH;
      CheckSum_c += (FdataH<<8);
      
      Fdata=FdataH*0x100+FdataL;

      //Fdata=Fdata & 0x3fff;
      //2012.09.12 dynamic id
       mcu_addr=OTP_ADDR/2;
      
      OTP_ADDR=OTP_ADDR+2;						
      //------------------------------------------------
      Rxdata=Fdata;
      
      ReCnt=0;
      while(++ReCnt<6)  
      {	
        MC321_W(Fdata);		
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
            //test++;
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
          if(ERORR_VALUE!=NOMALL_ERROR&&ERORR_VALUE!=MAGRIN1_ERROR&&ERORR_VALUE!=OFF_MAGRIN_ERROR)
          {             
            ERORR_VALUE=ROM_Write_false;
          }
          OTP_ADDR= mcu_addr;
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
   
//-------------------------以下写OPTION相关代码----------------------- 
  //Write OPTION  
  if(DeviceConfig_xx.WriteEn & 0xf0)
  {
    FM_CS_0;
    FMReadMore(Addr_Flash_OptionProtec+1);
    Fdata =Rxdata;
    
    ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr); //set ADDR
    for(op_addr=0;op_addr<DeviceConfig_xx.OptionSize;op_addr++)//
    {
      SPI_ReadByte();//从flash中取一个字节
      FdataL=Rxdata;
      //CheckSum +=FdataL;
      SPI_ReadByte();//再从flash中取一个字节
      FdataH=Rxdata;
      //CheckSum +=FdataH;
      Fdata=FdataH*0x100+FdataL;//组成一个字
      
      //option1 //写频率校准值 和温度校准值  
      if(op_addr==1)
      {          
        ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);
        if(ROMReadData==0xffff)//没写过频率校准值和温度校准值
        {
          Fdata =(OPTION_FRT*256)|OPTION_FRT2;
        }
        else
        {
          Fdata = ROMReadData;
        }
      }
      else if(op_addr==3)//Vref
      {
        ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);
        if((ROMReadData&0x00ff) == 0x00ff)//没写过adc reference校准值 
        {
          ROMReadData = MC321_ReadByte(0x800A);//取成测adc reference校准值
          if((ROMReadData&0x00ff) == 0x00ff)//没有成测adc reference校准值
          {
            ROMReadData = MC321_ReadByte(0x8009);//取中测adc reference校准值
            if((ROMReadData&0x00ff) == 0x00ff)//也没有中测adc reference校准值
            {
              ERORR_VALUE=OPTION_Write_false;//没写过adc reference频率校准值 也没有成测adc reference值 也没有中测adc reference值 报错
              return 0;
            }
          }
        //  Fdata = (Fdata&0xff00)|(ROMReadData&0x00ff);//成测、或中测校准值           
        }
        else//写过adc reference校准值 
        {    
        }              
        Fdata = (Fdata&0xff00)|(ROMReadData&0x00ff);//成测、或中测校准值           
      }

      ReCnt=0;
      while(++ReCnt<6)  
      {	
        if (Fdata == 0xffff)  // 
        {
          ROMReadData=MC321_R();
          if(ROMReadData == 0xffff)
          {//不需要写
            break;
          }
        }
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
        if(i==3)//3种模式读取都和写入的数据一致 烧写成功 3
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
      //以下判断是否写完
      if (op_addr<DeviceConfig_xx.OptionSize-1)
      {//没有写完 rom地址加1 继续写
        MC301_Instruct(AddrINC);
      }
    }
    FM_CS_1;
  }
  
  //如果是选择内部RC 需要check rom
  /*
  ROMReadData=MC321_ReadByte_DiffMode(0,DeviceConfig_xx.OptionAddr+0+option_shift);
  if((ROMReadData&0x0400)==0)//内部RC
  {
    MCP42050_ADJ(ADJ_VDD,15); 
    Delay_1ms(10);
    CheckSum_7011 = GetCheckSum_7011(mcu_addr);
    if(CheckSum_7011 != CheckSum_c)
    {
      ERORR_VALUE=25;
      return 0;
    }
   
    MCP42050_ADJ(ADJ_VDD,(u8)DeviceConfig_xx.IrcVDD); 
    Delay_1ms(10);   
  }
  */
  //write checksum
  ReCnt=0;  
  while(++ReCnt<3)
  {
    if(option_shift==0x00)//2k或1k模式第二次烧录
    {
      MC321_WriteByte(0x800b,CheckSum);
    }
    else if(option_shift==0x05)//1k模式第一次烧录
    {
      MC321_WriteByte(0x800c,CheckSum);
    }
  }
  
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  return 1;  
}