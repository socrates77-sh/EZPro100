

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
#include "MC30P6060.h"

void Delay_10us(u16 cnt );

//6060
//OSCCAL是8位 TADJ是6位
void MC30P6060FTtest_8Bit(u16 OSCCAL,u8 TADJ)
{
  u16 temp;
  u8  adjH,adjL;
  u16 i;
  u16 cnt[3];
  u16   Fdata;
  //OPTION 1：
  //bit13:8 --  bit5:bit0 TADJ
  //bit7:0  --  bit7:0   OSCCAL
  
  Fdata = ((TADJ & 0x003f)<<8) + (OSCCAL & 0x00ff);
  adjH=(Fdata&0xff00)>>8; //H
  adjL=Fdata&0x00ff; //L  
  
  temp=0x0b00+adjL;
  MC321_IRC_INST(temp);
  temp=0x1580+0x7b;//0x7b opt1 L地址
  MC321_IRC_INST(temp); //option1 L

  temp=0x0b00+adjH;
  MC321_IRC_INST(temp); //
  temp=0x1580+0x7c;//0x7c opt1 H地址  
  MC321_IRC_INST(temp); //option1 H  
  
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
    TEST_IRC[TEST_COUNT++]=cnt[i];
  }
  IRC_VALUE= (u16)((cnt[0]+cnt[1]+cnt[2])/3 );
}
/*******************************************************************************
* Function Name  : ChangeTempIRC
* Description    : 用不同温度值校准
* Input          : temp：温度值；IRC_FreqType：频率中心值；
*                  IRC_FreqMin：频率最小值；IRC_FreqMax：频率最大值
* Output         : None
* Return         : Status
*******************************************************************************/

u8 ChangeTempIRC(u8 TADJ_temp,u16 IRC_FreqType,u16 IRC_FreqMin,u16 IRC_FreqMax)
{
  u8 i;
  u16 temp0_FT,temp1_FT;
  u16 OSCCAL_temp=0x80;//频率校准位先给中间值
  OSCCAL = 0xffff;
  TADJ = 0xff;
  for(i=0;i<8;i++)
  {
    MC30P6060FTtest_8Bit(OSCCAL_temp,TADJ_temp);
    if (IRC_VALUE == IRC_FreqType)
    {
      break;
    }
    if (IRC_VALUE<IRC_FreqType)
    {
      OSCCAL_temp |=0x80 >>i;
    } 
    else
    {
      OSCCAL_temp &=~(0x80>>i);
    }
    OSCCAL_temp |=0x40>>i;
  }
  MC30P6060FTtest_8Bit(OSCCAL_temp+1,TADJ_temp);
  temp0_FT=IRC_VALUE;  
  MC30P6060FTtest_8Bit(OSCCAL_temp,TADJ_temp);
  temp1_FT=IRC_VALUE;
  if((temp0_FT-IRC_FreqType)<(IRC_FreqType-temp1_FT))
  {
    OSCCAL_temp=OSCCAL_temp+1;
    IRC_VALUE=temp0_FT;
  }
  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    OSCCAL = OSCCAL_temp;
    TADJ = TADJ_temp;
    return 1;//当前温度值条件下校验成功，结束校验
  }
  return 0;
}
u8 MC30P6060IRC_8Bit(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i;
  u8 optL,optH,opt0L;
  u16 mm;
  u8 OPTION_FAS;
  u8 OPTION_TEMPADJ,OPTION_TEMPADJ_1,OPTION_TEMPADJ_2;
  
  FM_CS_0;
  
 
  
  if (FMReadOne(Addr_Flash_OptionProtec+8)==1)//取option 3 低字节 确定是16M 8M 4M 2M 1M 455K哪一种
  {
    optL =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    return 0;
  }    
  
  OPTION_FAS = optL & 0x07; //确定是16M 8M 4M 2M 1M 455K哪一种，再确定两次校验要用的温度校准值OPTION_TEMPADJ_1、OPTION_TEMPADJ_2、OPTION_TEMPADJ_3
  
  MC301_Instruct(TestMod);

  for(i=0;i<5;i++)
  {
    MC321_IRC_INST(0X0000); //5 NOP
  }
  
   //说明：上位机会根据用户选的FAS(内部高频振荡器频率选择位)给出FDS、RCSMTB、VSDEL相关位
  
  //{先设置测试条件，即写FAS、FDS、RCSMTB、VDSEL相关位
  //同时确定两次校验要用的温度校准值OPTION_TEMPADJ_1、OPTION_TEMPADJ_2及不同FAS对应IRC_FreqMin、IRC_FreqMax、IRC_FreqType
  
  MC321_IRC_INST(0x0b00+0xef);     //关闭开门狗
  MC321_IRC_INST(0x1580+0x7a);    
  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;  
  
  MC321_IRC_INST(0x0b00+0xff);     //正常驱动；使能端口施密特功能；端口施密特阈值选择1： 0.7VDD/0.3VDD
  MC321_IRC_INST(0x1580+0x7d);     
  
  
  if(OPTION_FAS == 0)//16M
  {
    OPTION_TEMPADJ = 0x09;
    IRC_FreqMin = 2480;
    IRC_FreqType = 2500;
    IRC_FreqMax = 2520; 
    opt0L = 0x20;//设置分频系数64
  }
  else if(OPTION_FAS == 1)//8M
  {
    OPTION_TEMPADJ = 0x12;
    IRC_FreqMin = 1240;
    IRC_FreqType = 1250;
    IRC_FreqMax = 1260; 
    opt0L = 0x20;//设置分频系数64
  }
  else if(OPTION_FAS == 2)//4M
  {
    OPTION_TEMPADJ = 0x18;
    IRC_FreqMin = 1240;
    IRC_FreqType = 1250;
    IRC_FreqMax = 1260;     
    opt0L = 0x40;//设置分频系数32
  }
  else if(OPTION_FAS == 3)//2M
  {
    OPTION_TEMPADJ = 0x2c;
    IRC_FreqMin = 2480;
    IRC_FreqType = 2500;
    IRC_FreqMax = 2520; 
    opt0L = 0x80;//设置分频系数8
  }   
  else if(OPTION_FAS == 4)//1M
  {
    OPTION_TEMPADJ = 0x16;
    IRC_FreqMin = 1240;
    IRC_FreqType = 1250;
    IRC_FreqMax = 1260; 
    opt0L = 0x80;//设置分频系数8
  }   
  else if(OPTION_FAS == 5)//455K
  {
    OPTION_TEMPADJ = 0x19;
    IRC_FreqMin = 2255;
    IRC_FreqType = 2275;
    IRC_FreqMax = 2295;       
    opt0L = 0xc0;//设置分频系数2
  }   
  //opt0L |= 0x1f;
  opt0L &= 0xe0;//LVR电压=1.2V;屏蔽外部复位功能
  mm=0x0b00+opt0L;
  MC321_IRC_INST(mm);
  mm=0x1580+0x79;//0x79：opt0 L地址
  MC321_IRC_INST(mm);     //opt0 L
  
  optL = (optL & 0x27) | 0xd8;//opt3 L FAS  RCSMTB相关位 注意：校验时 要求FDS要写入固定值11
  mm=0x0b00+optL;
  MC321_IRC_INST(mm);
  mm=0x1580+0x7f;//0x7f：opt3 L地址
  MC321_IRC_INST(mm);     //opt3 L
  
  if (FMReadOne(Addr_Flash_OptionProtec+7)==1)//option 2 H
  {
    optH =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }
  
  optH = (optH & 0x10) | 0xee;//VSDEL相关位 选择内部高频RC振荡器
  mm=0x0b00+optH;
  MC321_IRC_INST(mm);
  mm=0x1580+0x7e;//0x7e：opt2 H地址
  MC321_IRC_INST(mm);     //opt2 H
  
  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;

  MC321_IRC_INST(0x0b00+0x01);     //movai 0x01
  MC321_IRC_INST(0x1580+0x78);     //movra 0x78 //set HIRC output to SDO

  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;   
  
  //}
  //开始校验
  
  if(ChangeTempIRC(OPTION_TEMPADJ,IRC_FreqType,IRC_FreqMin,IRC_FreqMax))
  {
    return 1;
  }
  
  i=0;
  OPTION_TEMPADJ_1=OPTION_TEMPADJ;
  OPTION_TEMPADJ_2=OPTION_TEMPADJ;
  while(OPTION_TEMPADJ_1>0x01||OPTION_TEMPADJ_2<0x3f)
  {    
    i++;
    if(OPTION_TEMPADJ_1>0x01)
    {
      OPTION_TEMPADJ_1=OPTION_TEMPADJ-i;
      if(ChangeTempIRC(OPTION_TEMPADJ_1,IRC_FreqType,IRC_FreqMin,IRC_FreqMax))
      {
        return 1;
      } 
    }
    if(OPTION_TEMPADJ_2<0x3f)
    {
      OPTION_TEMPADJ_2=OPTION_TEMPADJ+i;
      if(ChangeTempIRC(OPTION_TEMPADJ_2,IRC_FreqType,IRC_FreqMin,IRC_FreqMax))
      {
        return 1;
      }     
    }
  }  
  //三次校验都失败
  OSCCAL=0xffff;
  TADJ=0xff;
  ERORR_VALUE=IRC_Value_false;
  return 0;
}
u8 MC30P6060IRC_8Bit_last(u16 IRC_FreqMin,u16 IRC_FreqMax)
{
  u8 i;
  u8 optL,optH,opt0L;
  u16 mm;
  u8 OPTION_FAS;
   
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(10);
  MC30P6060_MODEL_IN(vdd30);
  Delay_1ms(1);  
  
  FM_CS_0;
  if (FMReadOne(Addr_Flash_OptionProtec+8)==1)//取option 3 低字节 确定是16M 8M 4M 2M 1M 455K哪一种
  {
    optL =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    return 0;
  }    
  
  OPTION_FAS = optL & 0x07; //确定是16M 8M 4M 2M 1M 455K哪一种，再确定两次校验要用的温度校准值OPTION_TEMPADJ_1、OPTION_TEMPADJ_2、OPTION_TEMPADJ_3
  
  MC301_Instruct(TestMod);

  for(i=0;i<5;i++)
  {
    MC321_IRC_INST(0X0000); //5 NOP
  }
  if(OPTION_FAS == 0)//16M
  {
    IRC_FreqMin = 2480;
    IRC_FreqMax = 2520; 
    opt0L = 0x20;//设置分频系数64
  }
  else if(OPTION_FAS == 1)//8M
  {
    IRC_FreqMin = 1240;
    IRC_FreqMax = 1260; 
    opt0L = 0x20;//设置分频系数64
  }
  else if(OPTION_FAS == 2)//4M
  {
    IRC_FreqMin = 1240;
    IRC_FreqMax = 1260;     
    opt0L = 0x40;//设置分频系数32
  }
  else if(OPTION_FAS == 3)//2M
  {
    IRC_FreqMin = 2480;
    IRC_FreqMax = 2520; 
    opt0L = 0x80;//设置分频系数8
  }   
  else if(OPTION_FAS == 4)//1M
  {
    IRC_FreqMin = 1240;
    IRC_FreqMax = 1260; 
    opt0L = 0x80;//设置分频系数8
  }   
  else if(OPTION_FAS == 5)//455K
  {
    IRC_FreqMin = 2255;
    IRC_FreqMax = 2295;       
    opt0L = 0xc0;//设置分频系数2
  }   
  opt0L |= 0x1f;
  mm=0x0b00+opt0L;
  MC321_IRC_INST(mm);
  mm=0x1580+0x79;//0x79：opt0 L地址
  MC321_IRC_INST(mm);     //opt0 L
  
  optL = (optL & 0x27) | 0xd8;//opt3 L FAS  RCSMTB相关位 注意：校验时 要求FDS要写入固定值11
  mm=0x0b00+optL;
  MC321_IRC_INST(mm);
  mm=0x1580+0x7f;//0x7f：opt3 L地址
  MC321_IRC_INST(mm);     //opt3 L
  
  if (FMReadOne(Addr_Flash_OptionProtec+7)==1)//option 2 H
  {
    optH =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }
  
  optH = (optH & 0x10) | 0xee;//VSDEL相关位 选择内部高频RC振荡器
  mm=0x0b00+optH;
  MC321_IRC_INST(mm);
  mm=0x1580+0x7e;//0x7e：opt2 H地址
  MC321_IRC_INST(mm);     //opt2 H
  
  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;

//  MC321_IRC_INST(0x0b2e) ; //选择内部高频RC振荡器
//  MC321_IRC_INST(0x15fa) ; //

  MC321_IRC_INST(0x0b00+0x01);     //movai 0x01
  MC321_IRC_INST(0x1580+0x78);     //movra 0x78 //set HIRC output to SDO

  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;   
  
  MC30P6060FTtest_8Bit(OSCCAL,TADJ);
  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    return 1;//当前温度值条件下校验成功，结束校验
  }  
  
  OSCCAL=0xffff;
  TADJ=0xff;
  ERORR_VALUE=IRC_Value_false;
  return 0;
}
u8 MC30P6060_Program()
{
  u8  i,ReCnt=0;
  u16  Fdata,ROMReadData;
  u16 RomEndAddr_6060;
  u8	FdataL,FdataH;
  u8  ReFlag=0;
  u16  CheckSum=0;
  u8  MCU_empty_flag=0;
  u8  option_shift=0;
  
  u8    id_length=0;
  u16   id_addr0,id_addr1,id_addr2,id_addr3,mcu_addr;
  u32   id_end,current_id,max_id;  
               
  //power on & model in
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(10);
  MC30P6060_MODEL_IN(vdd30);
  Delay_1ms(1);
  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=0x6060) 
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  }    
  
//查空
  /*
  if (DeviceConfig_xx.MCU_ID==0x6060)
  {
    ROMReadData=MC321_ReadByte(0x0000); //set ADDR
    for(OTP_ADDR=DeviceConfig_xx.RomFirAddr;OTP_ADDR<=DeviceConfig_xx.RomEndAddr;)
    {
      OTP_ADDR=OTP_ADDR+2;
      for(i=0;i<3;i++)
      {
        ROMReadData=MC321_R_DiffMode(i);
        if (ROMReadData==0x3fff)
        {
        }
        else
        {
          ERORR_VALUE=CHECK0_ERROR;       
          return 0;
        }
      } 
      if(i==3)//3种模式读取都和写入的数据一致 烧写成功 1
      {
        ReCnt=0;
      } 
      if (OTP_ADDR<DeviceConfig_xx.RomEndAddr)
      {
        MC301_Instruct(AddrINC);
      }        
    }
  }
  */
  //读芯片 是否写过 
  FM_CS_0;
  
  ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr);
  ROMReadData &= 0x0007;
  
  FMReadOne(0x012120+0); 
  Rxdata&=0x07;

  if(ROMReadData == 3)//用过 0.5K字节MTP模式，第一次烧录
  {
    if(Rxdata==0x04)//想再用 1K字节MTP模式 直接报错
    {
      ERORR_VALUE=ROM_Write_false;
      return 0; 
    }    
  }
  else if(ROMReadData == 2)//用过 0.5K字节MTP模式，第二次烧录 
  {
    if(Rxdata==0x04||Rxdata==0x03)//想再用 1K字节MTP模式  或0.5K字节MTP模式，第一次烧录  直接报错
    {
      ERORR_VALUE=ROM_Write_false;
      return 0; 
    }  
  }
  else if(ROMReadData == 4)//用过 1K字节MTP模式
  {
    if(Rxdata==0x03||Rxdata==0x02)//想再用 0.5K字节MTP模式  直接报错
    {
      ERORR_VALUE=ROM_Write_false;
      return 0; 
    }    
  }   
  /*
  if(ROMReadData!=0x0007 && ROMReadData == Rxdata)//芯片同样page被写过
  {
    if(Rxdata == 0x04 ||Rxdata == 0x02)//1k或第二次0.5k
    {
      option_shift = 0; 
    }
    else if(Rxdata == 0x03)//第一次0.5k
    {
      option_shift = 4;
    }

    Fdata=MC321_ReadByte(0X8004+option_shift);
    if (FMReadOne(Addr_Flash_OptionProtec+8)==1)//读   
    {
      //optL =Rxdata;
    }
    else
    {
      ERORR_VALUE=FM_Read_false;
      FM_CS_1;
      return 0;
    }    
 //   OPTION_FAS = Rxdata & 0x07;   
    if(Fdata != 0x3fff)//芯片同样的0.5或1k如果被写过 判断这次用的FAS和上次用的是否一致 不一致 直接报错
    {
      FdataL = Fdata%256;
      FdataL &= 0x07;
      if((Rxdata & 0x07) != FdataL)//选用的内部高频震荡器频率不一致 
      {
        ERORR_VALUE=HIRC_DIFF;//直接报错
        return 0;
      }
    }  
  }
  */
  FM_CS_1;
  //IRC adjust
  if (DeviceConfig_xx.IRC_Select==0x01) //check adjust, 01 need adjust 
  {
    if (DeviceConfig_xx.MCU_ID==0x6060)
    {
      TEST_COUNT=0;
      ReFlag=MC30P6060IRC_8Bit(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
      ConfigTIM4();
      Delay_1ms(10);
      if(ReFlag==1)//需要将校准得到的频率校准值再送进芯片校一次
      {
        TEST_COUNT=0;
        ReFlag=MC30P6060IRC_8Bit_last(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax);
        ConfigTIM4();
      }      
    }
    if(ReFlag == 0)
    {
      ERORR_VALUE=IRC_Value_false;
      return 0;
    }
  }
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(10);

  MC30P6060_MODEL_IN(vdd30);
  //MC6060_MODEIN(0xaeed);  
  Delay_1ms(1);
  Fdata=MC321_ReadByte(0XFFFF);
  CLN_VDD2GND_Off;
  VPP12V_On;
//  VPP_IO_On;
  Delay_1ms(10);
  if (Fdata !=0x6060) 
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  }
//Write Page

  FMReadOne(0x012120+0);
  FdataL = Rxdata;  
  FMReadOne(0x012120+1);
  FdataH = Rxdata;  
  Fdata=FdataH*0x100+FdataL;

  //Fdata = 0x0053;
  DeviceConfig_xx.OptionAddr = 0x8000;
  ReCnt = 0x00;
  while(++ReCnt<6)
  {		
    MC321_WriteByte(DeviceConfig_xx.OptionAddr,Fdata);
    for(i=0;i<3;i++)
    {
      ROMReadData=MC321_ReadByte_DiffMode(i,DeviceConfig_xx.OptionAddr);
      if (ROMReadData==(Fdata & 0x3fff))
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
      OTP_ADDR=DeviceConfig_xx.OptionAddr;
    //  ERORR_VALUE=OPTION_Write_false;
      if(ERORR_VALUE!=NOMALL_ERROR&&ERORR_VALUE!=MAGRIN1_ERROR&&ERORR_VALUE!=OFF_MAGRIN_ERROR)
      {             
        ERORR_VALUE=OPTION_Write_false;
      }       
      return 0;
    }                 
  }            
  //FdataL = 0x52;
  if(FdataL == 0xf2)  //the secand write 0.5k
  {
    option_shift = 0x0;
    RomEndAddr_6060 = 0x3ff;
  }
  else if(FdataL == 0xf3)  //the first write 0.5k
  {
    option_shift = 0x04;
    RomEndAddr_6060 = 0x3ff;
  }                
  else if((FdataL == 0xf5) || (FdataL == 0xf4))//write 1k
  {
    option_shift = 0x0;
    RomEndAddr_6060 = 0x7ff;
  }
  else
  {
    return 0;
  }                                
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(10);

  MC30P6060_MODEL_IN(vdd30);
  //MC6060_MODEIN(0xaeed);  
  Delay_1ms(1);
  Fdata=MC321_ReadByte(0XFFFF);
  CLN_VDD2GND_Off;
  VPP12V_On;
//  VPP_IO_On;
  Delay_1ms(10);

//Write ROM
  //滚动码开启
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
  Fdata=MC321_ReadByte(0X00ff); 
  Fdata=MC321_ReadByte(0X0100);   
  Fdata=MC321_ReadByte(0X01ff);
  Fdata=MC321_ReadByte(0X0200); 
  Fdata=MC321_ReadByte(0X03ff);   
  Fdata=MC321_ReadByte(0X0400);  
  if (DeviceConfig_xx.WriteEn & 0x0f)
  {
    CheckSum=0;
    FM_CS_0;

    DeviceConfig_xx.RomFirAddr = 0x0000; //set ADDR
    RomEndAddr_6060=RomEndAddr_6060;

    //FMReadMore(Addr_Flash_ROMStart+DeviceConfig_xx.RomFirAddr-1); //set flash address
    FMReadMore(Addr_Flash_ROMStart+0-1); //set flash address
    ROMReadData=MC321_ReadByte(DeviceConfig_xx.RomFirAddr); //set OTP address
    for(OTP_ADDR=DeviceConfig_xx.RomFirAddr;OTP_ADDR<=RomEndAddr_6060;)
    {
      SPI_ReadByte();
      FdataL=Rxdata;
      CheckSum +=FdataL;
      SPI_ReadByte();
      FdataH=Rxdata;
      CheckSum +=FdataH;
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
      //------------------------------------------------
      Rxdata=Fdata;
      //Fdata = 0xffff;
      ReCnt=0;
      while(++ReCnt<6)  //start wr rom
      {	
        MCU_empty_flag=0;
        if((DeviceConfig_xx.MCU_ID==0x6060)&&(Fdata==0xffff))
        {
          Fdata=0x3fff;
          MCU_empty_flag=1;
        }
        if ((Fdata!=0xffff) && (MCU_empty_flag==0))//(DeviceConfig_xx.MCU_Type==0x02))
        {
          MC321_W(Fdata); //wr data to rom
        }			
        ROMReadData=0x0000;
        for(i=0;i<3;i++)
        {
          ROMReadData=MC321_R_DiffMode(i);
          if (ROMReadData==(Fdata & 0x3FFF))
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
          return 0;
        }               
      }
      //if (OTP_ADDR<DeviceConfig_xx.RomEndAddr)
      if (OTP_ADDR<RomEndAddr_6060)
      {
        MC301_Instruct(AddrINC);
      }
    }
    FM_CS_1;
  }                    
//Write Option
  //Option 1
  FMReadOne(0x012120+4);
  //FdataL = Rxdata;  
//  FdataL = OPTION_FRT2;
  FdataL = OSCCAL;//校准得到的频率校准值
  FMReadOne(0x012120+5);
  FdataH = Rxdata;  
  FdataH = TADJ;//校准得到的温度校准值
  Fdata=FdataH*0x100+FdataL;
  DeviceConfig_xx.OptionAddr = (0x8002+option_shift);

  ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr); //check empty
  /*
  if (((ROMReadData) | 0x3F00) != 0x3fff)
    Fdata = ((Fdata & 0x3F00) | (ROMReadData & 0x00FF));
  */
  if(ROMReadData!= 0x3fff)//被写过
  {
    Fdata =ROMReadData;
  }
  ReCnt = 0x00;
  while(++ReCnt<6)
  {		
    MC321_WriteByte(DeviceConfig_xx.OptionAddr,Fdata);
    for(i=0;i<3;i++)
    {
      ROMReadData=MC321_ReadByte_DiffMode(i,DeviceConfig_xx.OptionAddr);
      if (ROMReadData==(Fdata&0x3fff))
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
                
  //Option 2
  FMReadOne(0x012120+6);
  FdataL = Rxdata;  
  FMReadOne(0x012120+7);
  FdataH = Rxdata;  
  Fdata=FdataH*0x100+FdataL;
  DeviceConfig_xx.OptionAddr = (0x8003+option_shift);
  ReCnt = 0x00;
  while(++ReCnt<6)
  {		
    MC321_WriteByte(DeviceConfig_xx.OptionAddr,Fdata);
    for(i=0;i<3;i++)
    {
      ROMReadData=MC321_ReadByte_DiffMode(i,DeviceConfig_xx.OptionAddr);
      if (ROMReadData==(Fdata&0x3fff))
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
  //Option 3
  FMReadOne(0x012120+8);
  FdataL = Rxdata;  
  FMReadOne(0x012120+9);
  FdataH = Rxdata;  
  Fdata=FdataH*0x100+FdataL;
  DeviceConfig_xx.OptionAddr = (0x8004+option_shift);
  ReCnt = 0x00;
  while(++ReCnt<6)
  {		
    MC321_WriteByte(DeviceConfig_xx.OptionAddr,Fdata);   
    for(i=0;i<3;i++)
    {
      ROMReadData=MC321_ReadByte_DiffMode(i,DeviceConfig_xx.OptionAddr);
      if (ROMReadData==(Fdata&0x3fff))
      {
      //  ReCnt=0;
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
    if(i==3)//3种模式读取都和写入的数据一致 烧写成功 5
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
  //Option 0
  FMReadOne(0x012120+2);
  FdataL = Rxdata;  
  FMReadOne(0x012120+3);
  FdataH = Rxdata;  
  Fdata=FdataH*0x100+FdataL;
  DeviceConfig_xx.OptionAddr = (0x8001+option_shift);
  ReCnt = 0x00;
  while(++ReCnt<6)
  {		
    MC321_WriteByte(DeviceConfig_xx.OptionAddr,Fdata);
    for(i=0;i<3;i++)
    {
      ROMReadData=MC321_ReadByte_DiffMode(i,DeviceConfig_xx.OptionAddr);
      if (ROMReadData==(Fdata&0x3fff))
      {
      //  ReCnt=0;
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
    if(i==3)//3种模式读取都和写入的数据一致 烧写成功 6
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
  //write checksum
  ReCnt=0;  
  while(++ReCnt<3)
  {
    if(option_shift==0x00)//write 1k or the secand write 0.5k
    {
      MC321_WriteByte(0x800b,CheckSum);
    }
    else if(option_shift==0x04)//the first write 0.5k
    {
      MC321_WriteByte(0x800c,CheckSum);
    }
  }
  ReCnt=0;  
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  return 1;
}
void MC30P6060_MODEL_IN(u8 VddFlag)
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

//  VDD_IO_Off; //VDD IO PIN SIGNAL

  NOP;
  NOP;
  NOP;NOP;NOP;NOP;NOP;NOP;
//  VDD_IO_On;

  CLN_VDD2GND_Off;
  VPP12V_On;    //Chip version
  NOP;NOP;NOP;NOP;NOP;NOP;
//  VPP_IO_On;    //FPGA version
  Delay_1us(5);
  //Delay_100us(2);
  NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;

  OTP_SDI_1;
  OTP_SCK_1;
  NOP;
  OTP_SCK_0;
  NOP;
  OTP_SDI_0;
  OTP_SCK_1;
  NOP;
  OTP_SCK_0;
  NOP;
  OTP_SDI_1;
  OTP_SCK_1;
  NOP;
  OTP_SCK_0;
  NOP;
  OTP_SDI_1;
  OTP_SCK_1;
  NOP;
  OTP_SCK_0;
  NOP;//0b1011          
  OTP_SDI_0;
  OTP_SCK_1;
  NOP;
  OTP_SCK_0;
  NOP;
  OTP_SDI_1;
  OTP_SCK_1;
  NOP;
  OTP_SCK_0;
  NOP;
  OTP_SDI_1;
  OTP_SCK_1;
  NOP;
  OTP_SCK_0;
  NOP;
  OTP_SDI_1;
  OTP_SCK_1;
  NOP;
  OTP_SCK_0;
  NOP;//0b0111

  OTP_SDI_0;
  OTP_SCK_1;
  NOP;
  OTP_SCK_0;
  NOP;
  OTP_SDI_1;
  OTP_SCK_1;
  NOP;
  OTP_SCK_0;
  NOP;
  OTP_SDI_1;
  OTP_SCK_1;
  NOP;
  OTP_SCK_0;
  NOP;
  OTP_SDI_1;
  OTP_SCK_1;
  NOP;
  OTP_SCK_0;
  NOP;//0b0111

  OTP_SDI_0;
  OTP_SCK_1;
  NOP;
  OTP_SCK_0;
  NOP;
  OTP_SDI_1;
  OTP_SCK_1;
  NOP;
  OTP_SCK_0;
  NOP;
  OTP_SDI_0;
  OTP_SCK_1;
  NOP;
  OTP_SCK_0;
  NOP;
  OTP_SDI_1;
  OTP_SCK_1;
  NOP;
  OTP_SCK_0;
  NOP;//0b0101  
  OTP_SDI_0;
  Delay_1ms(1);
  NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
}

//--------------------------------------------------------
void MC30P6060FTtest(u8 IRC_Option)
{
  u16 temp=0x0b00;
  u16 i;
  u16 cnt[3];
  temp=temp+IRC_Option;
  MC321_IRC_INST(temp);
  MC321_IRC_INST(0x15fb);
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
