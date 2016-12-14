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
#include "MC32P7311.h"
#include "MC9033.h"

//IRC校验测试版
void MC32P7311FTtest_10Bit(u16 irc_adj_value)
{
  u8 optH,optL;
  u16 temp=0x3c00;
  u16 i;
  u16 cnt[3];
  //OSCCAL bit10:0 -> opt1 bit10:0
  optH = irc_adj_value/256;
  optL = irc_adj_value%256;
  if (FMReadOne(Addr_Flash_Option+3)==1)
  {
  }    
  optH = (optH&0x07)|(Rxdata&0xf8);//送不同频率校准值时，opt1 bit15:11保持不变
  
  temp = 0x3c00+optH;
  MC321_IRC_INST(temp) ; 
  MC321_IRC_INST(0x57fb) ; //opt0 H   
  
  temp = 0x3c00+optL;
  MC321_IRC_INST(temp) ; 
  MC321_IRC_INST(0x57fc) ; //opt0 L
  
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
/*
u8 MC32P7311IRC_10Bit(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i,optL,optH;
  u16 mm=0x80;
  //u8 temp;
   u16 temp0_FT,temp1_FT ;
  //power on & model in
  MC321_MODEL_IN(vdd30);
//  VDD_IO_On;
  Delay_1ms(10);
  MC301_Instruct(TestMod);
//  VDD_IO_Off;

  for(i=0;i<5;i++)
  {
          MC321_IRC_INST(0X0000); //5 NOP
  }

  //分频系数 opt0 L
  if (FMReadOne(Addr_Flash_Option+0)==1)
  {
      optL = Rxdata;
  }
  else
  {
      ERORR_VALUE=FM_Read_false;
      return 0;
  }   
  //optL = optL&0xEB;//64ms
  //optL = optL|0x08;
  optL = optL&0xE7;//16ms
  optL = optL|0x04;  
//  optL = optL&0xDF;
//  optL = optL|0xC0;
  optL = optL|0xE0;
//  optL = optL&0x1F;
  mm = 0x3c00+optL;
  MC321_IRC_INST(mm) ; 
  MC321_IRC_INST(0x57fe) ; //movra	0x1fe

  //HIRC温度校准位 opt2 H
  if (FMReadOne(Addr_Flash_Option+5)==1)
  {
      optH = Rxdata;
  }
  else
  {
      ERORR_VALUE=FM_Read_false;
      return 0;
  }     
  //optH = 0xCA;//因为上位机暂时没有TADJ，下位机先写一个能校到的TADJ值
  mm = 0x3c00+optH;
  MC321_IRC_INST(mm) ; 
  MC321_IRC_INST(0x57f9) ; //movra	0x1f9

  //HIRC电压选择 opt1 H
  if (FMReadOne(Addr_Flash_Option+3)==1)
  {
      optH = Rxdata;//之后送不同频率校准值时，这个值要用
  }
  else
  {
      ERORR_VALUE=FM_Read_false;
      return 0;
  }     
  mm = 0x3c00+optH;
  MC321_IRC_INST(mm) ; 
  MC321_IRC_INST(0x57fb) ; //movra	0x1fc        

  MC321_IRC_INST(0x3c01) ;  //movai	0x01
  MC321_IRC_INST(0x57ff) ; 
  MC321_IRC_INST(0x0000) ;
//  VDD_IO_On;
  Delay_1ms(200);//进模式后至少150msSDO才有波形
  OPTION_FRT=0; //initial 
  OPTION_FRT2=0;
  mm = 0x400;
  for(i=0;i<11;i++)
  {
          MC32P7311FTtest_10Bit(mm);
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

  MC32P7311FTtest_10Bit(mm+1);
  temp0_FT=IRC_VALUE;

  MC32P7311FTtest_10Bit(mm);
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
          OPTION_FRT=0XFF;
          OPTION_FRT2=0xff;
          return 0;
  }
}
*/

//IRC校准 正式版
u8 MC32P7311IRC_10Bit(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i,j,optL=0,optH;
  u16 mm=0x80;
  u16 temp0_FT,temp1_FT ;

  MC301_Instruct(TestMod);

  for(i=0;i<5;i++)
  {
    MC321_IRC_INST(0X0000); //5 NOP
  }

  optL &= 0xE7;//WDTT 16ms
  optL |= 0x04;  
  
  optL |= 0xE0;//FCPUS 128分频

  mm = 0x3c00+optL;
  MC321_IRC_INST(mm) ; 
  MC321_IRC_INST(0x57fe) ; //movra	0x1fe


//  mm = 0x3c00+optH;
//  MC321_IRC_INST(mm) ; 
//  MC321_IRC_INST(0x57f9) ; //movra	0x1f9 

  //HIRC电压选择 opt1 H
  if (FMReadOne(Addr_Flash_Option+3)==1)
  {
    optH = Rxdata;//之后送不同频率校准值时，这个值要用
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    return 0;
  }     
  mm = 0x3c00+optH;
  MC321_IRC_INST(mm) ; 
  MC321_IRC_INST(0x57fb) ;//VDSEL        

  MC321_IRC_INST(0x3c01) ;  
  MC321_IRC_INST(0x57ff) ; 
  MC321_IRC_INST(0x0000) ;
//  VDD_IO_On;
  Delay_1ms(200);//进模式后至少150msSDO才有波形
  OPTION_FRT=0; //initial 
  OPTION_FRT2=0;
  
  for(j=0;j<5;j++)//5个温度校准值 0x08 0x07 0x06 0x09 0x05
  {
    if(j==0)
    {
      TADJ=0x07;
    }
    else if(j==1)
    {
      TADJ=0x08;
    }
    else if(j==2)
    {
      TADJ=0x06;
    }
    else if(j==3)
    {
      TADJ=0x09;
    }
    else if(j==4)
    {
      TADJ=0x05;
    }    
    mm = 0x3c00+TADJ|0xc0;
    MC321_IRC_INST(mm) ; 
    MC321_IRC_INST(0x57f9) ; //movra	0x1f9 写温度校准值
    mm = 0x400;
    for(i=0;i<11;i++)
    {
      MC32P7311FTtest_10Bit(mm);
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

    MC32P7311FTtest_10Bit(mm+1);
    temp0_FT=IRC_VALUE;

    MC32P7311FTtest_10Bit(mm);
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
      return 1;//校准成功 退出
    } 
  }
  OSCCAL=0xff;
  TADJ=0xff;
  ERORR_VALUE=IRC_Value_false;
  return 0;
}


u8 MC32P7311IRC_10Bit_Check(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u16 mm=0x80;
     
  mm = 0x3c00+TADJ|0xc0;
  MC321_IRC_INST(mm) ; 
  MC321_IRC_INST(0x57f9) ; //movra	0x1f9 写温度校准值
  
  mm=OPTION_FRT*256+OPTION_FRT2;
  MC32P7311FTtest_10Bit(mm);

  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    //OPTION_FRT=mm/256;
    //OPTION_FRT2=mm%256;
    return 1;//校准成功 退出
  } 
  OSCCAL=0xff;
  TADJ=0xff;
  ERORR_VALUE=IRC_Value_false;
  return 0;
}

//IRC校准 正式版
u8 MC32P7311IRC_10Bit_ModeInCheck(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i,optL=0,optH;
  u16 mm=0x80;

  MC301_Instruct(TestMod);

  for(i=0;i<5;i++)
  {
    MC321_IRC_INST(0X0000); //5 NOP
  }

  optL &= 0xE7;//WDTT 16ms
  optL |= 0x04;  
  
  optL |= 0xE0;//FCPUS 128分频

  mm = 0x3c00+optL;
  MC321_IRC_INST(mm) ; 
  MC321_IRC_INST(0x57fe) ; //movra	0x1fe


//  mm = 0x3c00+optH;
//  MC321_IRC_INST(mm) ; 
//  MC321_IRC_INST(0x57f9) ; //movra	0x1f9 

  //HIRC电压选择 opt1 H
  if (FMReadOne(Addr_Flash_Option+3)==1)
  {
    optH = Rxdata;//之后送不同频率校准值时，这个值要用
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    return 0;
  }     
  mm = 0x3c00+optH;
  MC321_IRC_INST(mm) ; 
  MC321_IRC_INST(0x57fb) ;//VDSEL        

  MC321_IRC_INST(0x3c01) ;  
  MC321_IRC_INST(0x57ff) ; 
  MC321_IRC_INST(0x0000) ;
//  VDD_IO_On;
  Delay_1ms(200);//进模式后至少150msSDO才有波形

  mm = 0x3c00+TADJ|0xc0;
  MC321_IRC_INST(mm) ; 
  MC321_IRC_INST(0x57f9) ; //movra	0x1f9 写温度校准值
    
  mm=OPTION_FRT*256+OPTION_FRT2;
  MC32P7311FTtest_10Bit(mm);

  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    //OPTION_FRT=mm/256;
    //OPTION_FRT2=mm%256;
    return 1;//校准成功 退出
  } 
  OSCCAL=0xff;
  TADJ=0xff;
  ERORR_VALUE=IRC_Value_false;
  return 0;
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
//u8 MC32P7311_Program()
//{
//  u8  i,ReCnt=0,op_addr=0;
//  u16  Fdata,ROMReadData;
//  u8	FdataL,FdataH,option_shift=0;
//  u8  ReFlag=0;
//  u16  CheckSum=0;
//  u16   mcu_addr; 
//  
//  u8    id_length=0;
//  u16   id_addr0,id_addr1,id_addr2,id_addr3;
//  u32   id_end,current_id,max_id; 
//  
//  u8    j;
//  u8    ForceReCnt=0;  
//  u8    SuccessCnt;   
//  u16   Addres;  
//  
//  WriteConfig_xx.WriteCheckNeed=0;
//  WriteConfig_xx.ReadCnt=1;
//  WriteConfig_xx.ReadOkCnt=1;
//  WriteConfig_xx.ReWriteCnt=3;
//  WriteConfig_xx.ForceWriteCnt=1;
//  WriteConfig_xx.AllCheckNeed=1;
// 
////------------------------------先进模式----------------------------
//  VDD30V_Off;
//  POWER_OFF(vpp00,vdd00);
//  Delay_1ms(5);
//  //power on & model in
//  MC321_MODEL_IN(vdd30);
//  CLN_VDD2GND_Off;
//  VPP12V_On;
//  Fdata=MC321_ReadByte(0XFFFF);
//  if (Fdata !=DeviceConfig_xx.MCU_ID) 
//  {
//          ERORR_VALUE=OTP_ModeIn_false;
//          return 0;
//  }        
//  
////------------------------------IRC校准----------------------------        
//  if (DeviceConfig_xx.IRC_Select==0x01)
//  {
//    ReFlag=MC32P7311IRC_10Bit(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
//    if(ReFlag==1)
//    {
//      ReFlag=MC32P7311IRC_10Bit_Check(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
//    }
//    ConfigTIM4();
//    if(ReFlag == 0) 
//    {
//      ERORR_VALUE=IRC_Value_false;
//      return 0;
//    }
//  }  
//  
////------------------------------再进模式----------------------------
//  VDD30V_Off;
//  POWER_OFF(vpp00,vdd00);
//  Delay_1ms(5);
//  //power on & model in
//  MC321_MODEL_IN(vdd30);
//  CLN_VDD2GND_Off;
//  VPP12V_On;
//  Fdata=MC321_ReadByte(0XFFFF);
//  if (Fdata !=DeviceConfig_xx.MCU_ID) 
//  {
//          ERORR_VALUE=OTP_ModeIn_false;
//          return 0;
//  }    
//  
////------------------------------Write ROM---------------------------- 
//  if (DeviceConfig_xx.WriteEn & 0x0f)
//  {
//    CheckSum=0;
//    FM_CS_0;
//    FMReadMore(Addr_Flash_ROMStart+DeviceConfig_xx.RomFirAddr-1);
//    Fdata =Rxdata;
//
//    //MCU的芯片才可能会有滚动码这个需求    其他芯片不需要关于iic_data的代码
//    id_length=iic_data[1];
//    id_addr0=(iic_data[2]<<8)+iic_data[3];// ADDRL
//    id_addr1=(iic_data[4]<<8)+iic_data[5];
//    id_addr2=(iic_data[6]<<8)+iic_data[7];
//    id_addr3=(iic_data[8]<<8)+iic_data[9];//ADDRH
//    current_id=(iic_data[10]<<24)+(iic_data[11]<<16)+(iic_data[12]<<8)+iic_data[13];//当前已经烧写过的芯片写入的编号
//    id_end=(iic_data[15]<<24)+(iic_data[16]<<16)+(iic_data[17]<<8)+iic_data[18];//烧写成功的次数 限制值
//    max_id=(iic_data[27]<<24)+(iic_data[28]<<16)+(iic_data[29]<<8)+iic_data[30]; //max-id//芯片最大编号
//    if (current_id>max_id)
//    {
//      iic_data[10]=iic_data[23];
//      iic_data[11]=iic_data[24];
//      iic_data[12]=iic_data[25];
//      iic_data[13]=iic_data[26];
//    }
//    if (iic_data[0]==0x0a)//烧写次数限制功能打开
//    {
//      if (OKcounter>=id_end)//烧写成功的次数达到限制值
//      {
//        ERORR_VALUE=OK_Counter_Full;
//        return 0; // full limit
//      }
//    }
//    
//    ROMReadData=MC321_ReadByte(0x0000); //set ADDR
//    for(OTP_ADDR=DeviceConfig_xx.RomFirAddr;OTP_ADDR<=DeviceConfig_xx.RomEndAddr;)
//    {
//      SPI_ReadByte();
//      FdataL=Rxdata;
//      CheckSum +=FdataL;
//      SPI_ReadByte();
//      FdataH=Rxdata;
//      CheckSum +=FdataH;
//      Fdata=FdataH*0x100+FdataL;
//      //Fdata=Fdata & 0x3fff;
//      //2012.09.12 dynamic id
//      mcu_addr=OTP_ADDR/2;
//      OTP_ADDR=OTP_ADDR+2;						
//      if (iic_data[0]==0x0a) //id enable MCU的芯片才可能会有滚动码这个需求
//      {
//        if (mcu_addr==id_addr0)
//        {
//          //Fdata=iic_data[13];
//          Fdata=FdataH*0x100+iic_data[13]; // must keep the H byte for code
//        }
//        else if ((mcu_addr==id_addr1)&& (id_length>1))
//        {
//          //Fdata=iic_data[12];
//          Fdata=FdataH*0x100+iic_data[12];
//        }
//        else if ((mcu_addr==id_addr2)&& (id_length>2))
//        {
//          //Fdata=iic_data[11];
//          Fdata=FdataH*0x100+iic_data[11];
//        }
//        else if ((mcu_addr==id_addr3)&&(id_length>3))
//        {
//          //Fdata=iic_data[10];
//          Fdata=FdataH*0x100+iic_data[10];
//        }
//      }
//      //------------------------------------------------
//      Rxdata=Fdata;
//      
//      ReCnt=WriteConfig_xx.ReWriteCnt;
//      while(ReCnt--)  //start wr rom
//      {	 
//        ForceReCnt=WriteConfig_xx.ForceWriteCnt;
//        if (Fdata!=0xffff)
//        {
//          while(ForceReCnt--)
//          {
//            MC321_W(Fdata); //wr data to rom
//          }
//        }
//        SuccessCnt = 0;
//        if(WriteConfig_xx.WriteCheckNeed)
//        {
//          for(i=0;i<WriteConfig_xx.ReadCnt;i++)
//          {
//            ROMReadData=MC321_R();
//            if (ROMReadData==Fdata)
//            {
//              SuccessCnt++;
//            }
//            if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
//            {
//              break;
//            }
//          } 
//        }
//        else
//        {
//          SuccessCnt = WriteConfig_xx.ReadOkCnt;
//        }
//        
//        if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//在规定读取校验次数下达到所要求的成功次数 烧写成功
//        {
//          //ReCnt=0;
//          break;
//        }
//        else
//        {
//          ERORR_VALUE=ROM_Write_false;
//        }
//        
//        if (ReCnt==0)
//        {
//          OTP_ADDR= mcu_addr;
//          return 0;
//        }                    
//      }
//      if (OTP_ADDR<DeviceConfig_xx.RomEndAddr)
//      {
//        MC301_Instruct(AddrINC);
////        if (DeviceConfig_xx.MCU_ID==0x7311)
////        {
////          mcu_addr = MC321_PreRead(PAddrRe);
////          if (mcu_addr!=OTP_ADDR/2)
////          {
////            ERORR_VALUE=ADDR_EER;
////            return 0;
////          }
////        }
//      }      
//    }
//    FM_CS_1;
//  }  
//
////------------------------------Write OPTION ---------------------------- 
//  if(DeviceConfig_xx.WriteEn & 0xf0)
//  {
//    FM_CS_0;
//    FMReadMore(Addr_Flash_OptionProtec-1);
//    Fdata =Rxdata;
//    
//    for(op_addr=0;op_addr<DeviceConfig_xx.OptionSize;op_addr++)//
//    {
//      SPI_ReadByte();//从flash中取一个字节
//      FdataL=Rxdata;
//      //CheckSum +=FdataL;
//      SPI_ReadByte();//再从flash中取一个字节
//      FdataH=Rxdata;
//      //CheckSum +=FdataH;
//      Fdata=FdataH*0x100+FdataL;//组成一个字
//      
//      //option0 
//      if(op_addr==0)
//      {          
//        Fdata|=0x8000;//不写加密位
//      }      
//      //option1 //写频率校准值 
//      if(op_addr==1)
//      {          
//        ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr);
//        if((ROMReadData&0x7ff)==0x07ff)//没写过频率校准值
//        {
//          Fdata = (Fdata&0xf800)|(OPTION_FRT*256)|OPTION_FRT2;
//        }
//        else
//        {
//          Fdata = (Fdata&0xf800)|(ROMReadData&0x7ff);
//        }
//        //Fdata &= 0xDFFF;
//      }
//      else if(op_addr==2)//写温度校准值 及Vref
//      {//
//        ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr);
//        if((ROMReadData&0x00ff) == 0x00ff)//没写过adc reference校准值 
//        {
//          ROMReadData = (ROMReadData&0xff00)|(0x00ff&MC321_ReadByte(0x8005));//取成测adc reference校准值
//          if((ROMReadData&0x00ff) == 0x00ff)//没有成测adc reference校准值
//          {
//            ROMReadData = (ROMReadData&0xff00)|(0x00ff&MC321_ReadByte(0x8004));//取中测adc reference校准值
//            if((ROMReadData&0x00ff) == 0x00ff)//也没有中测adc reference校准值
//            {
//              ERORR_VALUE=OPTION_Write_false;//没写过adc reference频率校准值 也没有成测adc reference值 也没有中测adc reference值 报错
//              return 0;
//            }
//          }          
//        }      
//        if((ROMReadData&0x3f00) == 0x3f00)//没写过温度校准值 
//        {
//          ROMReadData = (ROMReadData&0xc0ff)|(TADJ<<8);
//        }        
//        Fdata = ROMReadData;//
//      }
//      else if(op_addr==3)//VBG
//      {//
//        ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr);
//        if((ROMReadData&0x00ff) == 0x00ff)//没写过VBG校准值 
//        {
//          ROMReadData = (ROMReadData&0xff00)|((0xff00&MC321_ReadByte(0x8005))>>8);//取成测VBG校准值
//          if((ROMReadData&0x00ff) == 0x00ff)//没有成测VBG校准值
//          {
//            ROMReadData = (ROMReadData&0xff00)|((0xff00&MC321_ReadByte(0x8004))>>8);//取中测VBG校准值
//            if((ROMReadData&0x00ff) == 0x00ff)//也没有中测VBG校准值
//            {
//              ERORR_VALUE=OPTION_Write_false;//没写过VBG频率校准值 也没有成测VBG值 也没有中测VBG值 报错
//              return 0;
//            }
//          }          
//        }        
//        Fdata = ROMReadData;//
//      }
//      WriteConfig_xx.OptionValue[op_addr] = Fdata;//烧option之前先把值存起来，以方便后续全面校验时读值
//      WriteConfig_xx.OptionAddr[op_addr]=DeviceConfig_xx.OptionAddr+op_addr+option_shift;
//      ReCnt=WriteConfig_xx.ReWriteCnt;
//      while(ReCnt--)  
//      {	
//        ForceReCnt=WriteConfig_xx.ForceWriteCnt;
//        if (Fdata != 0xffff)  // 
//        {
//          while(ForceReCnt--)
//          {
//            MC321_WriteByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift,Fdata);
//          }
//        }        
//	SuccessCnt = 0;
//	if (WriteConfig_xx.WriteCheckNeed)
//	{
//	  for(i=0;i<WriteConfig_xx.ReadCnt;i++)
//	  {
//	    ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);         
//	    if (ROMReadData==Fdata)
//	    {
//	      SuccessCnt++;
//	    }
//	    else
//	    {
//	    }
//	    if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
//	    {
//	      break;
//	    }
//	  }
//	}
//	else
//	{
//	  SuccessCnt = WriteConfig_xx.ReadOkCnt;
//	}
//        if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//在规定读取校验次数下达到所要求的成功次数 烧写成功
//        {
//          //ReCnt=0;
//          break;
//        }
//	else
//	{
//	  ERORR_VALUE=OPTION_NOMALL_ERROR;
//	}
//	
//        if (ReCnt==0)
//        {        
//          OTP_ADDR=DeviceConfig_xx.OptionAddr+op_addr+option_shift;
//          return 0;
//        }                     
//      }
//    }
//    FM_CS_1;
//  }  
//  
////------------------------------write checksum----------------------------   
//  ReCnt=0;  
//  while(++ReCnt<3)
//  {
//    MC321_WriteByte(0x8008,CheckSum);
//  }
//  
////------------------------------频率验证 取写入芯片的温度校准值和频率校准值----------------------------  
//  if (DeviceConfig_xx.IRC_Select==0x01)//正式代码
//  //if (DeviceConfig_xx.IRC_Select==0x00)//调试代码
//  {
//    ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+1+option_shift); //set ADDR
//    OPTION_FRT=ROMReadData/256;
//    OPTION_FRT&=0x07;
//    OPTION_FRT2=ROMReadData%256;
//    ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+2+option_shift); //set ADDR
//    TADJ=ROMReadData/256;
//    TADJ&=0x3f;
//    ReFlag=MC32P7311IRC_10Bit_ModeInCheck(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
//    ConfigTIM4();//关闭Timer3、Timer4
//    if(ReFlag == 0) 
//    {
//      ERORR_VALUE=IRC_Value_false;
//      return 0;
//    }
//  }   
//  
////------------------------------再进模式---------------------------- 
//  VDD30V_Off;
//  POWER_OFF(vpp00,vdd00);
//  Delay_1ms(1);    
//  MC321_MODEL_IN(vdd30);  
//  Delay_1ms(1);
//  CLN_VDD2GND_Off;
//  VPP12V_On;
//  Delay_1ms(10);  
//  Fdata=MC321_ReadByte(0XFFFF);
//  if (Fdata !=DeviceConfig_xx.MCU_ID) 
//  {
//    ERORR_VALUE=OTP_ModeIn_false;
//    return 0;
//  }
//  
////------------------------------rom 全面读校验----------------------------   
//  if (DeviceConfig_xx.WriteEn & 0x0f)
//  {
//    if (WriteConfig_xx.AllCheckNeed > 0)
//    {
//      FM_CS_0;
//      FMReadMore(Addr_Flash_ROMStart+DeviceConfig_xx.RomFirAddr-1);
//      Fdata =Rxdata;
//      
//      ROMReadData=MC321_ReadByte(0x0000); //set ADDR
//      for(OTP_ADDR=DeviceConfig_xx.RomFirAddr;OTP_ADDR<=DeviceConfig_xx.RomEndAddr;)
//      {
//	SPI_ReadByte();
//	FdataL=Rxdata;
//	SPI_ReadByte();
//	FdataH=Rxdata;
//	Fdata=FdataH*0x100+FdataL;
//	
//	mcu_addr=OTP_ADDR/2;
//	OTP_ADDR=OTP_ADDR+2;
//	
//	if (WriteConfig_xx.AllCheckNeed == 2)
//	{
//          for(i=1;i<3;i++)
//          {
//            SuccessCnt = 0;
//            for(j=0;j<WriteConfig_xx.ReadCnt;j++)
//            {
//              ROMReadData=MC321_R_DiffMode(i);
//              if (ROMReadData==Fdata)
//              {
//                SuccessCnt++;
//              }
//              else
//              {
//              }
//              if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
//              {
//                break;
//              }
//            } 
//            
//            if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//在规定读取校验次数下达到所要求的成功次数 烧写成功
//            {
//            }
//            else
//            {
//              if(i==1)
//              {
//                ERORR_VALUE=MAGRIN1_ERROR;
//              }
//              else if(i==2)//测试用
//              {
//                ERORR_VALUE=OFF_MAGRIN_ERROR;
//              }
//              break;
//            }
//          }  
//          if (i==3)//3种模式读取都和写入的数据一致 烧写成功 2
//          {
//            //break;
//          }
//          else
//          {
//            OTP_ADDR= mcu_addr;
//            return 0;
//          }
//	}
//	else
//        {
//          SuccessCnt = 0;
//          for(j=0;j<WriteConfig_xx.ReadCnt;j++)
//          {
//            ROMReadData=MC321_R();
//            if (ROMReadData==Fdata)
//            {
//              SuccessCnt++;
//            }
//            else
//            {
//            }
//            if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
//            {
//              break;
//            }
//          } 
//          
//          if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//在规定读取校验次数下达到所要求的成功次数 烧写成功
//          {
//            //break;
//          }
//          else
//          {
//            ERORR_VALUE=NOMALL_ERROR;
//            OTP_ADDR= mcu_addr;
//            return 0;
//          }
//        }
//	
//	if (OTP_ADDR<DeviceConfig_xx.RomEndAddr)
//	{
//	  MC301_Instruct(AddrINC);
//	}
//      }
//      FM_CS_1;
//    }
//  }
//  
////------------------------------option全面读校验----------------------------    
//  if(DeviceConfig_xx.WriteEn & 0xf0)
//  {
//    if(WriteConfig_xx.AllCheckNeed > 0)
//    {
//      for(op_addr=0;op_addr<DeviceConfig_xx.OptionSize;op_addr++)
//      {
//	Fdata=WriteConfig_xx.OptionValue[op_addr];
//        Addres=WriteConfig_xx.OptionAddr[op_addr];
//
//	if (WriteConfig_xx.AllCheckNeed == 2)
//	{
//          for(i=1;i<3;i++)
//          {
//            SuccessCnt = 0;
//            for(j=0;j<WriteConfig_xx.ReadCnt;j++)
//            {
//              ROMReadData=MC321_ReadByte_DiffMode(i,Addres);
//              if (ROMReadData==Fdata)
//              {
//                SuccessCnt++;
//              }
//              else
//              {
//              }
//              if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
//              {
//                break;
//              }
//            } 
//            if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//在规定读取校验次数下达到所要求的成功次数 烧写成功
//            {
//            }
//            else
//            {
//              if(i==1)
//              {
//                ERORR_VALUE=OPTION_MAGRIN1_ERROR;
//              }
//              else if(i==2)//测试用
//              {
//                ERORR_VALUE=OPTION_OFF_MAGRIN_ERROR;
//              }
//              break;
//            }
//          }
//          if (i==3)//3种模式读取都和写入的数据一致 烧写成功 2
//          {
//            
//          }
//          else
//          {
//            OTP_ADDR= Addres;
//            return 0;
//          } 
//	}
//	else
//        {
//          SuccessCnt = 0;
//	  for(j=0;j<WriteConfig_xx.ReadCnt;j++)
//	  {
//	    ROMReadData=MC321_ReadByte(Addres);
//	    if (ROMReadData==Fdata)
//	    {
//	      SuccessCnt++;
//	    }
//	    else
//	    {
//	    }
//	    if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
//	    {
//	      break;
//	    }
//	  } 
//	  if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//在规定读取校验次数下达到所要求的成功次数 烧写成功
//	  {
//            //break;
//	  }
//	  else
//	  {
//	    ERORR_VALUE=OPTION_NOMALL_ERROR;
//	    OTP_ADDR= Addres;
//            return 0;
//	  }
//        }
//      }
//    }
//  }
//  
////------------------------------write protect bit----------------------------    
//  if(DeviceConfig_xx.WriteEn & 0xf0)
//  {  
//    FM_CS_0;
//    FMReadMore(Addr_Flash_Option-1);
//    Fdata =Rxdata;
//    SPI_ReadByte();//从flash中取一个字节
//    FdataL=Rxdata;
//    SPI_ReadByte();//再从flash中取一个字节
//    FdataH=Rxdata;
//    Fdata=FdataH*0x100+FdataL;//组成一个字 
//
//    ReCnt=WriteConfig_xx.ReWriteCnt;
//    while(ReCnt--) 
//    {	
//      ForceReCnt=WriteConfig_xx.ForceWriteCnt;      
//      if (Fdata != 0xffff)  // 
//      {
//        while(ForceReCnt--)
//        {
//          MC321_WriteByte(DeviceConfig_xx.OptionAddr+option_shift,Fdata);
//        } 
//      } 
//      if (WriteConfig_xx.AllCheckNeed == 2)
//      {
//        for(i=1;i<3;i++)
//        {
//          SuccessCnt = 0;
//          for(j=0;j<WriteConfig_xx.ReadCnt;j++)
//          {
//            ROMReadData=MC321_ReadByte_DiffMode(i,DeviceConfig_xx.OptionAddr+option_shift);
//            if (ROMReadData==Fdata)
//            {
//              SuccessCnt++;
//            }
//            else
//            {
//            }
//            if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
//            {
//              break;
//            }
//          } 
//          if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//在规定读取校验次数下达到所要求的成功次数 烧写成功
//          {
//          }
//          else
//          {
//            if(i==1)
//            {
//              ERORR_VALUE=OPTION_MAGRIN1_ERROR;
//            }
//            else if(i==2)//测试用
//            {
//              ERORR_VALUE=OPTION_OFF_MAGRIN_ERROR;
//            }
//            break;
//          }
//        }
//        if (i==3)//3种模式读取都和写入的数据一致 烧写成功 2
//        {
//          break;
//        }
//        else
//        {
//          OTP_ADDR= DeviceConfig_xx.OptionAddr+option_shift;
//          return 0;
//        } 
//      }  
//      else if(WriteConfig_xx.AllCheckNeed == 1)
//      {
//        SuccessCnt = 0;
//        for(j=0;j<WriteConfig_xx.ReadCnt;j++)
//        {
//          ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+option_shift);
//          if (ROMReadData==Fdata)
//          {
//            SuccessCnt++;
//          }
//          else
//          {
//          }
//          if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
//          {
//            break;
//          }
//        } 
//        if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//在规定读取校验次数下达到所要求的成功次数 烧写成功
//        {
//          break;//退出while
//        }
//        else
//        {
//          ERORR_VALUE=OPTION_NOMALL_ERROR;
//          OTP_ADDR= DeviceConfig_xx.OptionAddr+option_shift;
//          return 0;
//        }
//      }
//    }
//  }  
//  FM_CS_1; 
//  
//  VDD30V_Off;
//  POWER_OFF(vpp00,vdd00);
//  return 1;  
//}
//

