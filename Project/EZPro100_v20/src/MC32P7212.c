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
#include "MC32P7212.h"
#include "MC9033.h"


void MC32P7212FTtest_11Bit(u16 irc_adj_value)
{
  u16 temp=0x0400;
  u8  adjH,adjL;
  u16 i;
  u16 cnt[3];
  //OPTION 3：
  //bit10:0  --  bit10:0   OSCCAL
  
  adjH=irc_adj_value/256; //H
  adjL=irc_adj_value%256; //L  
  
  temp=0x3c00+adjL;
  MC321_IRC_INST(temp);
  MC321_IRC_INST(0x57f8); //option3 L
  MC321_IRC_INST(0x0000);

  temp=0x3c00+adjH;
  MC321_IRC_INST(temp); //
  MC321_IRC_INST(0x57f7); //option3 H  
  
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


//以下是IRC测试版//2015/03/24 已经不是老版本的7212，而是A020，要看SVN上A020资料
u8 MC32P7212IRC_11Bit(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i;
  u8 optL,optH;
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
  
  if (FMReadOne(Addr_Flash_OptionProtec+5)==1)//取flash中 option 2 高字节
  {
    optH =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }    
  mm=0x3c00+optH;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57f9);     //写opt2 H  写温度调教值
  
  if (FMReadOne(Addr_Flash_OptionProtec+8)==1)//取flash中 option4 低字节
  {
    optL =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }    
  mm=0x3c00+optL;
  mm = mm | 0x18;//FDS=11
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57f6);     //写opt4 L
  
  OPTION_FAS = optL & 0x07; //确定是16M 8M 4M 2M 1M 455K哪一种

  if(OPTION_FAS == 0)//16M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;
    optL = 0x70;//opt2 L 分频系数
  }
  else if(OPTION_FAS == 1)//8M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;   
    optL = 0x60;//opt2 L
  }
  else if(OPTION_FAS == 2)//4M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
    optL = 0x50;//opt2 L
  }
  else if(OPTION_FAS == 3)//2M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
    optL = 0x40;//opt2 L
  }   
  else if(OPTION_FAS == 4)//1M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
    optL = 0x30;//opt2 L
  }   
  else if(OPTION_FAS == 5)//455K
  {
    IRC_FreqType = 2275;
    IRC_FreqMin = 2268;
    IRC_FreqMax = 2282;    
    optL = 0x10;//opt2 L
  }   
  else//上位机没有配置OPTION 
  {
      return 0;
  }
  mm=0x3c00+optL;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fa);     //写opt2 L   
  
  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;

  MC321_IRC_INST(0x3c01);     //movai 0x01
  MC321_IRC_INST(0x57ff);     //movra 0x1ff //set HIRC output to SDO

  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;   

  mm=0x400;//频率校准位先给中间值
  OPTION_FRT=0; //initial 
  OPTION_FRT2=0; 
  for(i=0;i<11;i++)
  {
    MC32P7212FTtest_11Bit(mm);
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
  MC32P7212FTtest_11Bit(mm);
  temp1_FT=IRC_VALUE;
  MC32P7212FTtest_11Bit(mm+1);
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


//以下是IRC校验正式版 2015/03/24 lp说暂时还没有出内部RC校准说明，但可以参考7022/5312
//2015/03/24 已经不是老版本的7212，而是A020，要看SVN上A020资料
/*
u8 MC32P7212IRC_11Bit(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i,j;
  u8 optL,optH,opt2L,Fdata_H;
  u16 mm,temp,Fdata,temp0_FT,temp1_FT;
  u8 OPTION_FAS;
  u8 OPTION_TEMPADJ_1,OPTION_TEMPADJ_2;
  
  FM_CS_0;
  mm=0x400 ;//& (rdv<<8);
  //power on & model in
  MC321_MODEL_IN(vdd30);
  Delay_1ms(1);
  MC301_Instruct(TestMod);
  for(i=0;i<5;i++)
  {
    MC321_IRC_INST(0X0000); //5 NOP
  }  
  //武文军要求加以下两条指令码
  MC321_IRC_INST(0x3c03);
  MC321_IRC_INST(0x57ae);     //
  
  if (FMReadOne(Addr_Flash_OptionProtec+7)==1)//取flash中 option 2 高字节
  {
    optH =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }    

  mm=0x3c00+optH;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fe);     //写opt2 H
  
  OPTION_FAS = optH & 0x07; //确定是16M 8M 4M 2M 1M 455K哪一种

  if(OPTION_FAS == 0)//16M
  {
    OPTION_TEMPADJ_1 = 0x0a;
    OPTION_TEMPADJ_2 = 0x09;
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;
    optL = 0x70;//opt0 L
//    optL = 0x00;//opt0 L
  }
  else if(OPTION_FAS == 1)//8M
  {
    OPTION_TEMPADJ_1 = 0x12;
    OPTION_TEMPADJ_2 = 0x11;
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;   
    optL = 0x60;//opt0 L
  }
  else if(OPTION_FAS == 2)//4M
  {
    OPTION_TEMPADJ_1 = 0x29;
    OPTION_TEMPADJ_2 = 0x2a;
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
    optL = 0x50;//opt0 L
  }
  else if(OPTION_FAS == 3)//2M
  {
    OPTION_TEMPADJ_1 = 0x3F;
    OPTION_TEMPADJ_2 = 0x3E;  
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
    optL = 0x40;//opt0 L
  }   
  else if(OPTION_FAS == 4)//1M
  {
    OPTION_TEMPADJ_1 = 0x3F;
    OPTION_TEMPADJ_2 = 0x3E;  
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
    optL = 0x30;//opt0 L
  }   
  else if(OPTION_FAS == 5)//455K
  {
    OPTION_TEMPADJ_1 = 0x3F;
    OPTION_TEMPADJ_2 = 0x3E;  
    IRC_FreqType = 2275;
    IRC_FreqMin = 2268;
    IRC_FreqMax = 2282;    
    optL = 0x10;//opt0 L
  }   
  else//上位机没有配置OPTION 
  {
      return 0;
  }
  mm=0x3c00+optL;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57f9);     //写opt0 L
  
  if (FMReadOne(Addr_Flash_OptionProtec+6)==1)//取flash中 option 2 低字节
  {
    optL =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }  
  mm=0x3c00+optL;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fd);     //写opt2 L
  
  if (FMReadOne(Addr_Flash_OptionProtec+5)==1)//取option 1 H
  {
    optH =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }
  mm=0x3c00+optH;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fc);     //写opt1 H
  
  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;

  MC321_IRC_INST(0x3c01);     //movai 0x01
  MC321_IRC_INST(0x57f8);     //movra 0x1f8 //set HIRC output to SDO

  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;   

//  Delay_1ms(250);//测试发现必须要延时 > 212 此问题已经解决 
  for(j=0;j<2;j++)
  {
    //以下是温度校准值最高位
    if(j==0)
    {
      OPTION_TEMPADJ = OPTION_TEMPADJ_1;          
    }
    else 
    {
      OPTION_TEMPADJ = OPTION_TEMPADJ_2;
    }
    optL = (OPTION_TEMPADJ&0x20)>>5;
    if(FMReadOne(Addr_Flash_OptionProtec+6)==1)//取flash中 option 2 低字节
    {
      optL = (Rxdata&0xfe)|optL;
    }
    else
    {
      ERORR_VALUE=FM_Read_false;
      FM_CS_1;
      return 0;
    }    
    temp=0x3c00+optL;
    MC321_IRC_INST(temp); //
    MC321_IRC_INST(0x57fd); //option2 L
    
    
    
    mm=0x400;//频率校准位先给中间值
    OPTION_FRT=0; //initial 
    OPTION_FRT2=0; 
    IRC_MODEL=0;
    for(i=0;i<11;i++)
    {
      MC32P7212FTtest_11Bit(mm,OPTION_TEMPADJ);
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
    MC32P7212FTtest_11Bit(mm,OPTION_TEMPADJ);
    temp1_FT=IRC_VALUE;
    MC32P7212FTtest_11Bit(mm+1,OPTION_TEMPADJ);
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
      OPTION_TEMPADJ = OPTION_TEMPADJ;
      return 1;//校验成功，结束校验
    }  
  }

  OPTION_FRT=0xff;//校验失败
  OPTION_FRT2=0xff;
  OPTION_TEMPADJ = 0xff;  
  return 0;
  //}校验结束
}
*/
/*******************************************************************************
* Function Name  : MC32T8132_Program
* Description    : MC32T8132有擦除功能，先判断本次操作是不是擦除，如果是擦除，就
*                  只进行擦除操作；是烧写程序，就不进行擦除操作，只烧写
* Input          : None
* Output         : None
* Return         : 1 :成功  0：失败
*******************************************************************************/
u8 MC32P7212_Program()
{
  u8    ReCnt=0,opt_shift;
  u16   Fdata,ROMReadData;
  u8	FdataL,FdataH;  
  u8    ReFlag=0;
  u16   CheckSum=0;
  u16   mcu_addr;   
  u32   op_addr;
  u8    i;
  u16   test;
  
//------------------------------先进模式----------------------------
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(1);  
  MC321_MODEL_IN(vdd30);  
  Delay_1ms(1);
  CLN_VDD2GND_Off;
  VPP12V_On;
  Delay_1ms(10);
  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID)
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  }
  
  ROMReadData = MC321_ReadByte(0xff8);  
  ROMReadData = MC321_ReadByte(0xff9);    
  ROMReadData = MC321_ReadByte(0xffa);  
  ROMReadData = MC321_ReadByte(0xffb);  
  ROMReadData = MC321_ReadByte(0xffc);  
  ROMReadData = MC321_ReadByte(0xffd);  
  ROMReadData = MC321_ReadByte(0xffe);    
  ROMReadData = MC321_ReadByte(0xfff);  
  
//---------------------------判断芯片是否被写过----------------------
  
//-------------------------以下校验相关代码----------------------- 
  if (DeviceConfig_xx.IRC_Select==0x01)//正式代码
//  if (DeviceConfig_xx.IRC_Select==0x00)//调试代码
  {
    ReFlag=MC32P7212IRC_11Bit(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
    ConfigTIM4();
    if(ReFlag == 0) 
    {
      ERORR_VALUE=IRC_Value_false;
      return 0;
    }
  }
  
//-------------------------以下写ROM相关代码----------------------- 
  //power off then power on & model in
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(1);    
  MC321_MODEL_IN(vdd30);  
  Delay_1ms(1);
  CLN_VDD2GND_Off;
  VPP12V_On;
  Delay_1ms(10);  
  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID) 
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  }

//Write Page
  
  FM_CS_0;
  if (FMReadOne(Addr_Flash_OptionProtec)==1)//
  {
    FdataL =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }
  if (FMReadOne(Addr_Flash_OptionProtec+1)==1)//
  {
    FdataH =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }    
  Fdata=FdataH*0x100+FdataL;//组成一个字  
  while(++ReCnt<6)  
  {	
    MC321_WriteByte(0xff8,Fdata);
    for(i=0;i<1;i++)
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
        else if(i==2)
        {
          ERORR_VALUE=OFF_MAGRIN_ERROR;
        }
        
        test++;
        break;
      }
    } 
    if(i==1)//3种模式读取都和写入的数据一致 烧写成功 2
    {
      ReCnt=0;
      break;
    }
    else if (ReCnt==4)
    {
      if(ERORR_VALUE!=NOMALL_ERROR&&ERORR_VALUE!=MAGRIN1_ERROR&&ERORR_VALUE!=OFF_MAGRIN_ERROR)
      {             
        ERORR_VALUE=OPTION_Write_false;
      }
      OTP_ADDR= mcu_addr;
      return 0;
    }        
  }
  if(Fdata&0x0002)//1次烧写模式
  {
    DeviceConfig_xx.RomEndAddr = 0x1fef;
    opt_shift = 0;
  }
  else//2次烧写模式
  {
    DeviceConfig_xx.RomEndAddr = 0xff7;
    if((Fdata&0x0007)==0x0005)//第1页
    {
      opt_shift = 0;
    }
    if((Fdata&0x0007)==0x0004)//第2页
    {
      opt_shift = 3;
    }    
  }
  FM_CS_1;  
  
//Write ROM
  //power off then power on & model in
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(1);    
  MC321_MODEL_IN(vdd30);  
  Delay_1ms(1);
  CLN_VDD2GND_Off;
  VPP12V_On;
  Delay_1ms(10);  
  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID) 
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  }

  if (DeviceConfig_xx.WriteEn & 0x0f)//
//  if(DeviceConfig_xx.WriteEn == 0)//测试
  {
    CheckSum=0;
    FM_CS_0;
    FMReadMore(Addr_Flash_ROMStart+DeviceConfig_xx.RomFirAddr-1);
    Fdata =Rxdata;
    
    ROMReadData=MC321_ReadByte(0x0000); //set ADDR
    for(OTP_ADDR=DeviceConfig_xx.RomFirAddr;OTP_ADDR<=DeviceConfig_xx.RomEndAddr;)
    {
      SPI_ReadByte();//从flash中取一个字节
      FdataL=Rxdata;
      CheckSum +=FdataL;
      SPI_ReadByte();//再从flash中取一个字节
      FdataH=Rxdata;
      CheckSum +=FdataH;
      Fdata=FdataH*0x100+FdataL;//组成一个字

      mcu_addr=OTP_ADDR/2;
      OTP_ADDR=OTP_ADDR+2;//已经读了两个字节数据 
      
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
        MC321_W(Fdata);
        for(i=0;i<1;i++)
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
            test++;
            break;
          }
        } 
        if(i==1)//3种模式读取都和写入的数据一致 烧写成功 2
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
      //以下判断是否写完
      if (OTP_ADDR<DeviceConfig_xx.RomEndAddr)
      {//没有写完 rom地址加1 继续写
        MC301_Instruct(AddrINC);
      }
    }
    FM_CS_1;
  }
 //-------------------------以下写OPTION相关代码----------------------- 
  //Write OPTION  
  if(DeviceConfig_xx.WriteEn & 0xf0)
  {
    FM_CS_0;
    FMReadMore(Addr_Flash_OptionProtec-1);
    Fdata =Rxdata;
    
    ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr); //set ADDR
    for(op_addr=0;op_addr<DeviceConfig_xx.OptionSize;op_addr++)//
    {
      SPI_ReadByte();//从flash中取一个字节
      FdataL=Rxdata;
      CheckSum +=FdataL;
      SPI_ReadByte();//再从flash中取一个字节
      FdataH=Rxdata;
      CheckSum +=FdataH;
      Fdata=FdataH*0x100+FdataL;//组成一个字
      
      //option3 //      
      if(op_addr==3)//写频率校准值 
      {          
        ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+opt_shift);
        if((ROMReadData&0x07ff)==0x7ff)//没写过频率校准值
        {
          Fdata = (Fdata&0xf800)|(OPTION_FRT*256)|OPTION_FRT2;
        }
        else
        {
          Fdata = (Fdata&0xf800) | (ROMReadData&0x07ff);
        }
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
        if(op_addr<2)//OPBIT0~1: 4K程序模式和2K程序模式下都只可烧录一次。
        {
          MC321_WriteByte(DeviceConfig_xx.OptionAddr+op_addr,Fdata);
        }
        else
        {
          MC321_WriteByte(DeviceConfig_xx.OptionAddr+op_addr+opt_shift,Fdata);
        }
        for(i=0;i<1;i++)
        {
          if(op_addr<2)//OPBIT0~1: 4K程序模式和2K程序模式下都只可烧录一次。
          {
            ROMReadData=MC321_ReadByte_DiffMode(i,DeviceConfig_xx.OptionAddr+op_addr);
          }
          else
          {
            ROMReadData=MC321_ReadByte_DiffMode(i,DeviceConfig_xx.OptionAddr+op_addr+opt_shift);
          }
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
        if(i==1)//3种模式读取都和写入的数据一致 烧写成功 3
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
  
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  return 1;
}