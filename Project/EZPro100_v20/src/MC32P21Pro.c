
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
#include "MC9033.h"
#include "MC30P6060.h"
#include "MC32T8132.h"
#include "MC32E22.h" 
#include "MC32P7022.h"
#include "MC32P7212.h"
#include "MC32P7011.h"
#include "MC33P116Pro.h"
#include "MC32P5222Pro.h"

u8 MC32P21_Program_New()
{
  if(DeviceConfig_xx.MCU_ID==0x3316||DeviceConfig_xx.MCU_ID==0x3378||DeviceConfig_xx.MCU_ID==0x6060
    ||DeviceConfig_xx.MCU_ID==0x3111||DeviceConfig_xx.MCU_ID==0x3401||DeviceConfig_xx.MCU_ID==0x5312
     ||DeviceConfig_xx.MCU_ID==0x7022||DeviceConfig_xx.MCU_ID==0x7011
       ||DeviceConfig_xx.MCU_ID==0x7212||DeviceConfig_xx.MCU_ID==0x7712
        ||DeviceConfig_xx.MCU_ID==0x7511||DeviceConfig_xx.MCU_ID==0x6070||DeviceConfig_xx.MCU_ID==0x6080
          ||DeviceConfig_xx.MCU_ID==0x6090||DeviceConfig_xx.MCU_ID==0x7323||DeviceConfig_xx.MCU_ID==0x7311
            ||DeviceConfig_xx.MCU_ID==0x5222)//编程时间 90us - 110us
  {
    WriteConfig_xx.ProTime=1;
  }
  else if(DeviceConfig_xx.MCU_ID==0x9029||DeviceConfig_xx.MCU_ID==0x9033)
  {
    WriteConfig_xx.ProTime=2;
  }
  else 
  {
    WriteConfig_xx.ProTime=0;
  }
  if ((DeviceConfig_xx.MCU_ID==0x3401)||(DeviceConfig_xx.MCU_ID==0x3081)
  ||(DeviceConfig_xx.MCU_ID==0x3316)||(DeviceConfig_xx.MCU_ID==0x6060)||(DeviceConfig_xx.MCU_ID==0x5312)
    ||(DeviceConfig_xx.MCU_ID==0x7022)||(DeviceConfig_xx.MCU_ID==0x5222)
      ||(DeviceConfig_xx.MCU_ID==0x3316)||(DeviceConfig_xx.MCU_ID==0x8132)||(DeviceConfig_xx.MCU_ID==0x7011)
        ||(DeviceConfig_xx.MCU_ID==0x7212)||(DeviceConfig_xx.MCU_ID==0x7712)||(DeviceConfig_xx.MCU_ID==0x7511)
          ||(DeviceConfig_xx.MCU_ID==0x7323)||(DeviceConfig_xx.MCU_ID==0x6070)||(DeviceConfig_xx.MCU_ID==0x6090)
            ||(DeviceConfig_xx.MCU_ID==0x6080)||(DeviceConfig_xx.MCU_ID==0x9033))
  {
    WriteConfig_xx.ReadProAddrAndData=1;
  }   
  else 
  {
    WriteConfig_xx.ReadProAddrAndData=0;
  }
          
  if(DeviceConfig_xx.MCU_ID==0x6060)
  {
    if(MC30P6060_Program())//
    {
      return 1;
    }
  }
  else if(DeviceConfig_xx.MCU_ID==0x8132)
  {
    if(MC32T8132_Program())//
    {
      return 1;
    }
  } 
//  else if(DeviceConfig_xx.MCU_ID==0x3222)
//  {
//    if(MC32E22_Program())//
//    {
//      return 1;
//    }
//  } 
//  else if(DeviceConfig_xx.MCU_ID==0x7022)
//  {
//    if(MC32P7022_Program())//
//    //if(MC32P21_Program())//以前的
//    {
//      return 1;
//    }
//  }
//  else if(DeviceConfig_xx.MCU_ID==0x7712)
//  {
//    if(MC32P7212_Program())//
//    //if(MC32P21_Program())//以前的
//    {
//      return 1;
//    }
//  }
  else if(DeviceConfig_xx.MCU_ID==0x7011)
  {
    if(MC32P7011_Program())
    {
      return 1;
    }
  }
//  else if(DeviceConfig_xx.MCU_ID==0x7311)
//  {
//    if(MC32P7311_Program())
//    //if(MC32P7011_Program())
//    {
//      return 1;
//    }
//  }  
//  else if((DeviceConfig_xx.MCU_ID==0x7511)||(DeviceConfig_xx.MCU_ID==0x5222))
//  {
//    if(MC32P7511_Program())
//    {
//      return 1;
//    }
//  }  
//  else if(DeviceConfig_xx.MCU_ID==0x5312)
//  {
//    if(MC32P5312_Program())//
//    {
//      return 1;
//    }    
//  }
  else if(DeviceConfig_xx.MCU_ID==0x3316||DeviceConfig_xx.MCU_ID==0x5312||DeviceConfig_xx.MCU_ID==0x3111
          ||DeviceConfig_xx.MCU_ID==0x7311||DeviceConfig_xx.MCU_ID==0x7022)
  {
    if(MC33P116_Program())//
    {
      return 1;
    }    
  }  
  else if(DeviceConfig_xx.MCU_ID==0x5222)
  {
    if(MC32P5222_Program())
    {
      return 1;
    }
  }    
  else
  {
    if(MC32P21_Program())//以前的
    {
      return 1;
    }
  }
  return 0;
}
u8 MC32P21_Program()
{
  u8    i,RMNum,ReCnt=0;
  u16   Fdata,ROMReadData;
  u16   OldIRC_M301=0,OldIRC_word2=0;
  u8	FdataL,FdataH;
  u8    ReFlag=0;
  u8    MCU_empty_flag=0;
  u16   CheckSum=0,CheckAddr;

  u8    id_length=0;
  u16   id_addr0,id_addr1,id_addr2,id_addr3,mcu_addr;
  u32   id_end,current_id,max_id;
  u16   test=0;
  
  //IRC adjust
  if (DeviceConfig_xx.IRC_Select==0x01)//正式代码
  //if (DeviceConfig_xx.IRC_Select==0x00)//调试代码
  {
    if ((DeviceConfig_xx.MCU_ID==0x3394)||(DeviceConfig_xx.MCU_ID==0x3378))
    {
      ReFlag=M321IRC_11Bit(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
      if((DeviceConfig_xx.MCU_ID==0x3378)&&(ReFlag==1))//需要将校准得到的频率校准值再送进芯片校一次
      {
        ReFlag=M321IRC_11Bit_last(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
      }
    }
    else if (DeviceConfig_xx.MCU_ID==0x9029)
    {
      ReFlag=M9029IRC(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
      if(ReFlag==1)
      {
        ReFlag=M9029IRC_last(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
      }
    }
//    else if ((DeviceConfig_xx.MCU_ID==0x3111)||(DeviceConfig_xx.MCU_ID==0x3316))
//    {
//      ReFlag=M31P11IRC_11Bit(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
////      if((DeviceConfig_xx.MCU_ID==0x3111)&&(ReFlag==1))//需要将校准得到的频率校准值再送进芯片校一次
////      {
////        ReFlag=M31P11IRC_11Bit_last(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
////      }
//    }
    else if (DeviceConfig_xx.MCU_ID==0x5312)
    {
      ReFlag=MC32P5312IRC_11Bit(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
      if(ReFlag == ERORR_VALUE)
      {
        ERORR_VALUE=IRC_Value_false;
        return 0;
      }
    }
    
    else if ((DeviceConfig_xx.MCU_ID==0x3081)||(DeviceConfig_xx.MCU_ID==0x3401))
    {
      ReFlag=MC3081IRC(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
    }
    else
    {
      ReFlag=M321IRC(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
    }	
    if(ReFlag == 0) 
    {
      ERORR_VALUE=IRC_Value_false;
      return 0;
    }
  }      
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  ConfigTIM4();
  Delay_1ms(5);
  //power on & model in
  if (((DeviceConfig_xx.MCU_ID==0x3394)||(DeviceConfig_xx.MCU_ID==0x3378)||DeviceConfig_xx.MCU_ID==0x9029||DeviceConfig_xx.MCU_ID==0x3111)
  ||(DeviceConfig_xx.MCU_ID==0x3401)||(DeviceConfig_xx.MCU_ID==0x3316)||(DeviceConfig_xx.MCU_ID==0x5312)||(DeviceConfig_xx.MCU_ID==0x5188))
  {
    MC321_MODEL_IN(vdd30);
  }
  else if ((DeviceConfig_xx.MCU_ID==0x3264) && (DeviceConfig_xx.MCU_Type==0x02))
  {
    MC321_MODEL_IN(vdd30);
  }
  else
  {
    MC321_MODEL_IN(vdd65);
  }  
  Delay_1ms(1);
  CLN_VDD2GND_Off;
  VPP12V_On;
//  VPP_IO_On;
  Delay_1ms(10);
  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID)
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  }
  
  //ROMReadData=MC321_ReadByte(0x8005);
  //adjust if chip=mc33p74?
  if(DeviceConfig_xx.MCU_ID==0x3378)
  {
    Fdata=MC321_ReadByte(0x8008); // 33p74_adr8008=3374
    if (Fdata==0x3374)
    {
      if (DeviceConfig_xx.IRC_OPTION!=0x74)
      {
       return 0;
      }
    }
    else
    {
      if (DeviceConfig_xx.IRC_OPTION==0x74)
      {
         return 0;
      }
    }
  }        
  if (DeviceConfig_xx.MCU_Type==0x02) // disable the protected bit
  {
    ReCnt=0;
    Fdata=0xffff;
    while(++ReCnt<6)
    {		
      MC321_WriteByte(DeviceConfig_xx.OptionAddr,Fdata);
      ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr);
      if ( ROMReadData==Fdata)
      {
        ReCnt=0;
        break;
      }
      else if (ReCnt==4)
      {
        //POWER_OFF(vpp00,vdd00);
        OTP_ADDR=DeviceConfig_xx.OptionAddr;
        ERORR_VALUE=OPTION_Write_false;
        return 0;
      }
    }
    while(++ReCnt<6)
    {		
      MC321_WriteByte(DeviceConfig_xx.OptionAddr+1,Fdata);
      ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+1);
      if ( ROMReadData==Fdata)
      {
        ReCnt=0;
        break;
      }
      else if (ReCnt==4)
      {
        //POWER_OFF(vpp00,vdd00);
        OTP_ADDR=DeviceConfig_xx.OptionAddr;
        ERORR_VALUE=OPTION_Write_false;
        return 0;
      }
    }
    while(++ReCnt<6)
    {		
      MC321_WriteByte(DeviceConfig_xx.OptionAddr+2,Fdata);
      ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+2);
      if ( ROMReadData==Fdata)
      {
        ReCnt=0;
        break;
      }
      else if (ReCnt==4)
      {
        //POWER_OFF(vpp00,vdd00);
        OTP_ADDR=DeviceConfig_xx.OptionAddr;
        ERORR_VALUE=OPTION_Write_false;
        return 0;
      }
    }
    VDD30V_Off;
    POWER_OFF(vpp00,vdd00);
    ConfigTIM4();
    Delay_1ms(10);

    MC321_MODEL_IN(vdd30);
    Delay_1ms(1);

    CLN_VDD2GND_Off;
    VPP12V_On;;
    Delay_1ms(10);

    Fdata=MC321_ReadByte(0XFFFF);
    if (Fdata !=DeviceConfig_xx.MCU_ID)
    {
      ERORR_VALUE=OTP_ModeIn_false;
      return 0;
    }
  }
  
//check empty
/*
  if (DeviceConfig_xx.MCU_ID==0x3378||DeviceConfig_xx.MCU_ID==0x3316||DeviceConfig_xx.MCU_ID==0x9029
    ||DeviceConfig_xx.MCU_ID==0x3111||DeviceConfig_xx.MCU_ID==0x3401||DeviceConfig_xx.MCU_ID==0x5312)
  {
    ROMReadData=MC321_ReadByte(0x0000); //set ADDR
    for(OTP_ADDR=DeviceConfig_xx.RomFirAddr;OTP_ADDR<=DeviceConfig_xx.RomEndAddr;)
    {
      OTP_ADDR=OTP_ADDR+2;
      for(i=0;i<3;i++)
      {
        ROMReadData=MC321_R_DiffMode(i);
        if(DeviceConfig_xx.MCU_ID==0x3111||DeviceConfig_xx.MCU_ID==0x3401)
        {
          test = 0x1fff;
        }
        else 
        {
          test = 0xffff;
        }
        if (ROMReadData==test)
        {
        }
        else
        {
          ERORR_VALUE=CHECK0_ERROR;       
          test++;
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
        if ((DeviceConfig_xx.MCU_ID==0x3401)||(DeviceConfig_xx.MCU_ID==0x3081)||(DeviceConfig_xx.MCU_ID==0x3316))
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
  }
  */
  if (DeviceConfig_xx.MCU_ID==0x3316)
  {
    ReCnt=0;
    //Fdata=0xffff;
    id_end=addr_Flash_OptionProtec16K;
    if (FMReadOne(id_end)==1)
    {
      FdataL =Rxdata;
    }
    else
    {
      return 0;
    }
    Fdata=FdataL;
    while(++ReCnt<6)
    {		
      MC321_WriteByte(0x8000,Fdata);
      if((Fdata&0x70)==0x30)//16k
      {
        CheckAddr = 0x800D;
      }
      else if((Fdata&0x70)==0x50)//8k
      {
        if((Fdata&0x0C)==0x04)
        {
          CheckAddr = 0x800C;//第一页
        }
        else
        {
          CheckAddr = 0x800D;//第二页
        }
      }
      else if((Fdata&0x70)==0x60)//4k模式             
      {
        CheckAddr = 0;     //4K烧录不支持checksum   
//        if((Fdata&0x0f)==0x07)
//        {
//          CheckAddr = 0x800C;//第一页
//        }
//        else
//        {
//          CheckAddr = 0x800D;//第二页
//        }
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
          else if(i==2)
          {
            ERORR_VALUE=OFF_MAGRIN_ERROR;
          }
          
          test++;
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
        if((ERORR_VALUE!=NOMALL_ERROR)&&(ERORR_VALUE!=MAGRIN1_ERROR)&&(ERORR_VALUE!=OFF_MAGRIN_ERROR))//测试用
        {             
          ERORR_VALUE=OPTION_Write_false;
        }       
        return 0;
      }       
    }
    VDD30V_Off;
    POWER_OFF(vpp00,vdd00);
    Delay_1ms(10);

    MC321_MODEL_IN(vdd30);
    Delay_1ms(1);

    CLN_VDD2GND_Off;
    VPP12V_On;
    //VPP_IO_On;
    Delay_1ms(10);
    Fdata=MC321_ReadByte(0XFFFF);
    if (Fdata !=DeviceConfig_xx.MCU_ID)
    {
      ERORR_VALUE=OTP_ModeIn_false;
      return 0;
    }
  } //end of mc33p116 rom select funtion    
//Write ROM
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
      //------------------------------------------------
      Rxdata=Fdata;

      ReCnt=0;
      while(++ReCnt<6)  
      {	
        MCU_empty_flag=0;
        if((DeviceConfig_xx.MCU_ID==0x3111)&&(Fdata==0xffff))
        {
          Fdata &=0x1fff;
          MCU_empty_flag=1;
        }
        if((DeviceConfig_xx.MCU_ID==0x3401)&&(Fdata==0xffff))
        {
          Fdata=0x1fff;
          MCU_empty_flag=1;
        }
        if((DeviceConfig_xx.MCU_ID==0x3081)&&(Fdata==0xffff))
        {
          Fdata=0x3fff;
          MCU_empty_flag=1;
        }
        if(Fdata==0xffff) //other mcu 
        {
          MCU_empty_flag=1;
        }
        if(DeviceConfig_xx.MCU_ID==0x3081)//使用滚动码时，必须要
        {
          Fdata&=0x3fff;//
        }   
        if(DeviceConfig_xx.MCU_ID==0x3111||DeviceConfig_xx.MCU_ID==0x3401)//使用滚动码时，必须要
        {
          Fdata&=0x1fff;//
        }           
        if (MCU_empty_flag==0)  // data!=blank
        {
          MC321_W(Fdata);
        }		
        //mc31P11和mc33p78不只用普通读取 判断是否写成功 还要用Margin -1 Margin -2两种模式读 三种模式读出来都和写的数据一致 才说明写成功
        if(DeviceConfig_xx.MCU_ID==0x3111||DeviceConfig_xx.MCU_ID==0x3378||DeviceConfig_xx.MCU_ID==0x3401||DeviceConfig_xx.MCU_ID==0x3316
           ||DeviceConfig_xx.MCU_ID==0x5312||DeviceConfig_xx.MCU_ID==0x9029)
        {  
          if(DeviceConfig_xx.MCU_ID==0x9029)
          {
            RMNum = 2;
          }
          else
          {
            RMNum = 3;
          }
          for(i=0;i<RMNum;i++)
          {
            if(DeviceConfig_xx.MCU_ID==0x9029)
            {
              ROMReadData=MC9033_R_DiffMode(i);
            }
            else
            {
              ROMReadData=MC321_R_DiffMode(i);
            }
            if (ROMReadData==Fdata)
            {
              //ReCnt=0;
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
          if(i==RMNum)//3种模式读取都和写入的数据一致 烧写成功 2
          {
            ReCnt=0;
            break;
          }
          else if (ReCnt==4)
          {
            if((ERORR_VALUE!=NOMALL_ERROR)&&(ERORR_VALUE!=MAGRIN1_ERROR)&&(ERORR_VALUE!=OFF_MAGRIN_ERROR))//测试用
            {             
              ERORR_VALUE=ROM_Write_false;
            }
            OTP_ADDR= mcu_addr;
            return 0;
          }        
        }
        else
        {
          ROMReadData=MC321_R();   
          if (ROMReadData==Fdata)
          {
            ReCnt=0;
            break;
          }
          else if (ReCnt==4)
          {
            ERORR_VALUE=ROM_Write_false;
            OTP_ADDR= mcu_addr;
            return 0;
          }
        }
      }
      if (OTP_ADDR<DeviceConfig_xx.RomEndAddr)
      {
        MC301_Instruct(AddrINC);
        if ((DeviceConfig_xx.MCU_ID==0x3401)||(DeviceConfig_xx.MCU_ID==0x3081)||(DeviceConfig_xx.MCU_ID==0x3316))
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
    //mc9029
    if (DeviceConfig_xx.MCU_ID==0x9029)
    {
      //addr=0xf2--0xff
      FM_CS_0;
      FMReadMore(Addr_Flash_ROMStart+0x1e3);
      ROMReadData=MC321_ReadByte(0x00f2); //set ADDR
      for(OTP_ADDR=0x1e4;OTP_ADDR<=0x1ff;)
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
        Rxdata=Fdata;
        ReCnt=0;
        while(++ReCnt<6) 
        {		
          if (Fdata!=0xffff)
          {
            MC321_W(Fdata);
          }	
          for(i=0;i<2;i++)
          {
            ROMReadData=MC9033_R_DiffMode(i);
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
          if(i==2)//3种模式读取都和写入的数据一致 烧写成功 3
          {
            ReCnt=0;
            break;
          }
          else if (ReCnt==4)
          {
            if((ERORR_VALUE!=NOMALL_ERROR)&&(ERORR_VALUE!=MAGRIN1_ERROR)&&(ERORR_VALUE!=OFF_MAGRIN_ERROR))//测试用
            {             
              ERORR_VALUE=ROM_Write_false;
            }
            OTP_ADDR= mcu_addr;
            return 0;
          }       
          
        }
        if (OTP_ADDR<0x1ff)
        {
          MC301_Instruct(AddrINC);
        }
      }
      FM_CS_1;
      Fdata=OPTION_FRT2; // 0xf0
      ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr);
      if(ROMReadData==0xff)
      {
        while(++ReCnt<6)
        {		
          MC321_WriteByte(DeviceConfig_xx.OptionAddr,Fdata);
          for(i=0;i<2;i++)
          {
            ROMReadData=MC9033_ReadByte_DiffMode(i,DeviceConfig_xx.OptionAddr);
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
          if(i==2)//3种模式读取都和写入的数据一致 烧写成功 4
          {
            ReCnt=0;
            break;
          }
          else if (ReCnt==4)
          {
            if((ERORR_VALUE!=NOMALL_ERROR)&&(ERORR_VALUE!=MAGRIN1_ERROR)&&(ERORR_VALUE!=OFF_MAGRIN_ERROR))//测试用
            {             
              ERORR_VALUE=ROM_Write_false;
            }
            OTP_ADDR=DeviceConfig_xx.OptionAddr;
         //   ERORR_VALUE=OPTION_Write_false;
            return 0;
          }                 
        }
      }
      //0xf1
      id_end=0x0101e2;
      if (FMReadOne(id_end)==1)
      {
        Fdata =Rxdata;
      }
      else
      { 
        ERORR_VALUE=FM_Read_false;
        return 0;
      }
      Fdata &=0x80;
      //OPTION_FRT |=0x80;
      Fdata |= OPTION_FRT;
      ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+1);
      if (ROMReadData==0xff)
      {
        while(++ReCnt<6)
        {		
          MC321_WriteByte(DeviceConfig_xx.OptionAddr+1,Fdata);
          for(i=0;i<2;i++)
          {
            ROMReadData=MC9033_ReadByte_DiffMode(i,DeviceConfig_xx.OptionAddr+1);
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
          if(i==2)//3种模式读取都和写入的数据一致 烧写成功 5
          {
            ReCnt=0;
            break;
          }
          else if (ReCnt==4)
          {
            if((ERORR_VALUE!=NOMALL_ERROR)&&(ERORR_VALUE!=MAGRIN1_ERROR)&&(ERORR_VALUE!=OFF_MAGRIN_ERROR))//测试用
            {             
              ERORR_VALUE=ROM_Write_false;
            }            
            OTP_ADDR=DeviceConfig_xx.OptionAddr+1;
          //  ERORR_VALUE=OPTION_Write_false;
            return 0;
          }                 
        }
      }
    }//9029
  }  
  //Write Option
  if(DeviceConfig_xx.WriteEn & 0xf0)
  {
    //protect option
    if((DeviceConfig_xx.MCU_ID==0x3221)||(DeviceConfig_xx.MCU_ID==0x7510)||(DeviceConfig_xx.MCU_ID==0x3264)||(DeviceConfig_xx.MCU_ID==0x3111)
    ||(DeviceConfig_xx.MCU_ID==0x3081)||(DeviceConfig_xx.MCU_ID==0x3401)||(DeviceConfig_xx.MCU_ID==0x5188))
      id_end=0x012120;
    else
      id_end=Addr_Flash_OptionProtec8k;           
    if(DeviceConfig_xx.MCU_ID==0x5188)
    {
      //0x8000
      if (FMReadOne(id_end)==1)
      {
        FdataL =Rxdata;
      }
      else
      {
        return 0;
      }
      Fdata=0XFF00+FdataL;
      DeviceConfig_xx.OptionAddr=0X8000;
      while(++ReCnt<6)
      {		
        MC321_WriteByte(DeviceConfig_xx.OptionAddr,Fdata);
        ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr);
        if ( ROMReadData==Fdata)
        {
          ReCnt=0;
          break;
        }
        else if (ReCnt==4)
        {
          //POWER_OFF(vpp00,vdd00);
          OTP_ADDR=DeviceConfig_xx.OptionAddr;
          ERORR_VALUE=OPTION_Write_false;
          return 0;
        }

      }               
      //0x8001
      if (FMReadOne(id_end+1)==1)
      {
        FdataL =Rxdata;
      }
      else
      {
        return 0;
      }
      Fdata=0XFF00+FdataL;        
      while(++ReCnt<6)
      {		
        MC321_WriteByte(DeviceConfig_xx.OptionAddr+1,Fdata);
        ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+1);
        if ( ROMReadData==Fdata)
        {
          ReCnt=0;
          break;
        }
        else if (ReCnt==4)
        {
          //POWER_OFF(vpp00,vdd00);
          OTP_ADDR=DeviceConfig_xx.OptionAddr+1;
          ERORR_VALUE=OPTION_Write_false;
          return 0;
        }
      }             
      //0x8002
      if (FMReadOne(id_end+2)==1)
      {
        FdataL =Rxdata;
      }
      else
      {
        return 0;
      }
      Fdata=0XFF00+FdataL;              
      while(++ReCnt<6)
      {		
        MC321_WriteByte(DeviceConfig_xx.OptionAddr+2,Fdata);
        ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+2);
        if ( ROMReadData==Fdata)
        {
          ReCnt=0;
          break;
        }
        else if (ReCnt==4)
        {
          //POWER_OFF(vpp00,vdd00);
          OTP_ADDR=DeviceConfig_xx.OptionAddr+2;
          ERORR_VALUE=OPTION_Write_false;
          return 0;
        }
      }
      //0x8003
      if (FMReadOne(id_end+3)==1)
      {
        FdataL =Rxdata;
      }
      else
      {
        return 0;
      }
      Fdata=0XFF00+FdataL;     
      while(++ReCnt<6)
      {		
        MC321_WriteByte(DeviceConfig_xx.OptionAddr+3,Fdata);
        ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+3);
        if ( ROMReadData==Fdata)
        {
          ReCnt=0;
          break;
        }
        else if (ReCnt==4)
        {
          //POWER_OFF(vpp00,vdd00);
          OTP_ADDR=DeviceConfig_xx.OptionAddr+3;
          ERORR_VALUE=OPTION_Write_false;
          return 0;
        }
      }               
      //0x8004
      if (FMReadOne(id_end+4)==1)
      {
        FdataL =Rxdata;
      }
      else
      {
        return 0;
      }
      Fdata=0XFF00+FdataL;      
      while(++ReCnt<6)
      {		
        MC321_WriteByte(DeviceConfig_xx.OptionAddr+4,Fdata);
        ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+4);
        if ( ROMReadData==Fdata)
        {
          ReCnt=0;
          break;
        }
        else if (ReCnt==4)
        {
          //POWER_OFF(vpp00,vdd00);
          OTP_ADDR=DeviceConfig_xx.OptionAddr+4;
          ERORR_VALUE=OPTION_Write_false;
          return 0;
        }
      }               
      VDD30V_Off;
      POWER_OFF(vpp00,vdd00);
      return 1;             
    } //end of mc51p88
    if (DeviceConfig_xx.MCU_ID==0x3316)  //get the addr of option from flash
    {
      id_end=addr_Flash_OptionProtec16K+4;
    }           
    if(DeviceConfig_xx.MCU_ID==0x5312)
    {
      id_end=Addr_Flash_OptionProtec;//0x012120
    }
    if (FMReadOne(id_end)==1)
    {
      FdataL =Rxdata;
    }
    else
    {
      return 0;
    }
    if (FMReadOne(id_end+1)==1)
    {
      FdataH =Rxdata;
    }
    else
    {
      ERORR_VALUE=FM_Read_false;
      return 0;
    }			
    if (DeviceConfig_xx.MCU_ID==0x3111)
    {
      FdataH= FdataH & 0xf0;
      //FdataH= FdataH | 0x02;  //tempadj=0x04
      if (IRC_MODEL==0)// VDSEL=1, TEMPADJ=0X04
      {
        FdataH= FdataH | 0x02;  
        FdataL= FdataL & 0x7f; 
      }
      else if(IRC_MODEL==1)// VDSEL=1, TEMPADJ=0X05
      {
        FdataH= FdataH | 0x02;  
        FdataL= FdataL | 0x80;
      }
      else if(IRC_MODEL==2)// VDSEL=1, TEMPADJ=0X03
      {
        FdataH= FdataH | 0x01;  
        FdataL= FdataL | 0x80; 
      }
    }
    Fdata=FdataH*0x100+FdataL;
    //Fdata= Fdata & 0X3FFF;
    //write 0x2000 addr
    while(++ReCnt<6)
    {		
      MC321_WriteByte(DeviceConfig_xx.OptionAddr,Fdata);
      if(DeviceConfig_xx.MCU_ID==0x3111||DeviceConfig_xx.MCU_ID==0x3378||DeviceConfig_xx.MCU_ID==0x3401||DeviceConfig_xx.MCU_ID==0x3316
         ||DeviceConfig_xx.MCU_ID==0x5312)
      {
        for(i=0;i<3;i++)
        {
          ROMReadData=MC321_ReadByte_DiffMode(i,DeviceConfig_xx.OptionAddr);
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
        if(i==3)//3种模式读取都和写入的数据一致 烧写成功 6
        {
          ReCnt=0;
          break;
        }
        else if (ReCnt==4)
        {
          //POWER_OFF(vpp00,vdd00);
          if((ERORR_VALUE!=NOMALL_ERROR)&&(ERORR_VALUE!=MAGRIN1_ERROR)&&(ERORR_VALUE!=OFF_MAGRIN_ERROR))//测试用
          {             
            ERORR_VALUE=OPTION_Write_false;
          }                    
          OTP_ADDR=DeviceConfig_xx.OptionAddr;
        //  ERORR_VALUE=OPTION_Write_false;
          return 0;
        }         
      }
      else 
      {
        ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr);
        if ( ROMReadData==Fdata)
        {
          ReCnt=0;
          break;
        }
        else if (ReCnt==4)
        {
          //POWER_OFF(vpp00,vdd00);
          OTP_ADDR=DeviceConfig_xx.OptionAddr;
          ERORR_VALUE=OPTION_Write_false;
          return 0;
        }        
      }              
    }
    //0x8001
    if (DeviceConfig_xx.OptionSize>=2)
    {
      if (DeviceConfig_xx.IRC_Select==0x02) //mov 
      {
        ROMReadData=MC321_ReadByte(DeviceConfig_xx.IRC_VALUE_ADDR);
        OPTION_FRT=ROMReadData/256;
        OPTION_FRT2=ROMReadData%256;
        //OPTION_FRT=OPTION_FRT & 0x3ff;
        if ((OPTION_FRT == 0xff)||(OPTION_FRT==0x00))
        {
          ROMReadData=MC321_ReadByte(DeviceConfig_xx.IRC_VALUE_ADDR-2);
          OPTION_FRT=ROMReadData/256;
          OPTION_FRT2=ROMReadData%256;
          if ((OPTION_FRT == 0xff)||(OPTION_FRT==0x00))
          {
            ERORR_VALUE=IRC_Value_0XFF;
            return 0;
          }
        }
      }
      if (DeviceConfig_xx.MCU_ID==0x3081)
      {
        if (FMReadOne(id_end+6)==1) //read the shift data
        {
          MCU_empty_flag =Rxdata; //1,3,5,7,9,b
        }
        else
        {
          ERORR_VALUE=FM_Read_false;
          return 0;
        }
        OldIRC_M301=MC321_ReadByte(DeviceConfig_xx.OptionAddr+MCU_empty_flag);
      }
      else
      {
        OldIRC_M301=MC321_ReadByte(DeviceConfig_xx.OptionAddr+1);
        if(DeviceConfig_xx.MCU_ID==0x5312)//5312有两个option要写入校准值 只有这17个位都没有被写过才能写入新的校准值 
        {
          OldIRC_word2=MC321_ReadByte(DeviceConfig_xx.OptionAddr+2);
        }
      }
//OldIRC_M301=OldIRC_M301 & 0x00ff;
      if ((DeviceConfig_xx.MCU_ID==0x3111)||(DeviceConfig_xx.MCU_ID==0x3401))
      {
        Fdata=0x1fff;
      } 
      else if (DeviceConfig_xx.MCU_ID==0x3081)
      {
        Fdata=0x3fff;
      }
      else
      {
        Fdata=0xffff;
      }
      if(DeviceConfig_xx.MCU_ID==0x5312)
      {
        if((OldIRC_M301==Fdata)&&(OldIRC_word2&0x0001))//option 1 bit15:0和option 2 bit0为1（都没被写过）
        {
          FdataH =OPTION_FRT; //IRC from QC test
          FdataL =OPTION_FRT2;
          FdataH = (OPTION_TEMPADJ<<3)&0xf8 | OPTION_FRT ;
        }
        else
        {
          FdataL =OldIRC_M301;
          FdataH =(OldIRC_M301>>8);
        }
      }
      else
      {
        if(DeviceConfig_xx.MCU_ID==0x7510)
        {
          //频率校准值 
          if((OldIRC_M301&0x00ff) == 0x00ff)//没写过频率校准值 
          {
            FdataL =OPTION_FRT2;//IRC from QC test 
          }
          else //写过频率校准值
          {
            FdataL =OldIRC_M301;
          }
          
          //adc reference校准值
          if((OldIRC_M301&0x7f00) == 0x7f00)//没写过adc reference校准值 
          {
            ROMReadData = MC321_ReadByte(0x8006);//取成测adc reference校准值
            if((ROMReadData&0x7f00)==0x7f00)//没有成测adc reference校准值
            {
              ROMReadData = MC321_ReadByte(0x8005);//取中测adc reference校准值
              if((ROMReadData&0x7f00)==0x7f00)//也没有中测adc reference校准值
              {
                ERORR_VALUE=OPTION_Write_false;//没写过adc reference频率校准值 也没有成测adc reference值 也没有中测adc reference值 报错
                return 0;
              }
            }
            FdataH = ROMReadData/256;//成测、或中测校准值
            FdataH |=0x80; //bit15未用 值为1               
          }
          else//写过adc reference校准值 
          {
            FdataH =(OldIRC_M301>>8);          
          }              
        }
        else if ((OldIRC_M301==Fdata)&&(DeviceConfig_xx.MCU_ID!=0x3081)) //if 0x8001 is empty
        {
          FdataH =OPTION_FRT; //IRC from QC test
          FdataL =OPTION_FRT2;
          if (DeviceConfig_xx.MCU_ID==0x3401)
          {
            FdataL =OPTION_FRT; //IRC from QC test
          }                 
          if(DeviceConfig_xx.MCU_ID==0x3221||(DeviceConfig_xx.MCU_ID==0x7510))
          {
            Fdata=MC321_ReadByte(DeviceConfig_xx.IRC_VALUE_ADDR);
            FdataH=Fdata >>8;
            FdataL =OPTION_FRT2;
          }
          if (DeviceConfig_xx.MCU_ID==0x3316)
          {
            FdataH=OPTION_TEMPADJ & 0xf8;
            FdataH =FdataH | OPTION_FRT;
          }
          else if(DeviceConfig_xx.MCU_ID==0x5312)
          {
           // if(OldIRC_word2&0xfffe)//option 2 bit0为1（没被写过）
            {
              FdataH = (OPTION_TEMPADJ<<3)&0xf8 | OPTION_FRT ;
            }
          }
        }
        else if ((DeviceConfig_xx.MCU_ID==0x3081)&&(OldIRC_M301==Fdata))
        {
          FdataL=OPTION_FRT;

          if (FMReadOne(id_end+3)==1)
          {
            FdataH =Rxdata;
          }
          else
          {
            ERORR_VALUE=FM_Read_false;
            return 0;
          }
        }
        else
        {
          FdataL =OldIRC_M301;
          FdataH =(OldIRC_M301>>8);
        }
      }
      Fdata=FdataH*0x100+FdataL;
      if (DeviceConfig_xx.MCU_ID==0x3111)
      {
        if (FMReadOne(id_end+3)==1)
        {
          FdataH =Rxdata;
        }
        else
        {
          ERORR_VALUE=FM_Read_false;
          return 0;
        }
        FdataH= FdataH & 0x18;
        Fdata |=(FdataH*0x100);
      }
      else if ((DeviceConfig_xx.MCU_ID==0x3264)||(DeviceConfig_xx.MCU_ID==0x3401))
      {
        if (FMReadOne(id_end+3)==1)
        {
          FdataH =Rxdata;
        }
        else
        {
          ERORR_VALUE=FM_Read_false;
          return 0;
        }
        Fdata= Fdata & 0x00ff;
        Fdata |=(FdataH*0x100);
      }
      //Fdata=0x2f55;
      //write 0x2001 addr
      while(++ReCnt<6)
      {	
        if(DeviceConfig_xx.MCU_ID==0x3111||DeviceConfig_xx.MCU_ID==0x3378||DeviceConfig_xx.MCU_ID==0x3401||DeviceConfig_xx.MCU_ID==0x3316
           ||DeviceConfig_xx.MCU_ID==0x5312)
        {
          MC321_WriteByte(DeviceConfig_xx.OptionAddr+1,Fdata);          
          for(i=0;i<3;i++)
          {
            ROMReadData=MC321_ReadByte_DiffMode(i,DeviceConfig_xx.OptionAddr+1);
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
          if(i==3)//3种模式读取都和写入的数据一致 烧写成功 7
          {
            ReCnt=0;
            break;
          }
          else if (ReCnt==4)
          {
            if((ERORR_VALUE!=NOMALL_ERROR)&&(ERORR_VALUE!=MAGRIN1_ERROR)&&(ERORR_VALUE!=OFF_MAGRIN_ERROR))//测试用
            {             
              ERORR_VALUE=OPTION_Write_false;
            }                    
            OTP_ADDR=DeviceConfig_xx.OptionAddr+1;
            return 0;
          }         
        }
        else
        {
          if (DeviceConfig_xx.MCU_ID==0x3081)
          {
            MC321_WriteByte(DeviceConfig_xx.OptionAddr+MCU_empty_flag,Fdata);
            ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+MCU_empty_flag);
          }
          else
          {
            MC321_WriteByte(DeviceConfig_xx.OptionAddr+1,Fdata);
            ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+1);
          }
          if ( ROMReadData==Fdata)
          {
            ReCnt=0;
            break;
          }
          else if (ReCnt==4)
          {
            ERORR_VALUE=OPTION_Write_false;
            OTP_ADDR=DeviceConfig_xx.OptionAddr+1;
            return 0;
          }
        }
      }
      //wite 0X1FF2 /OPBIT2
      if ((DeviceConfig_xx.MCU_ID==0x3378)||(DeviceConfig_xx.MCU_ID==0x3394)||(DeviceConfig_xx.MCU_ID==0x3264)
      ||(DeviceConfig_xx.MCU_ID==0x3401)||(DeviceConfig_xx.MCU_ID==0x3081)||(DeviceConfig_xx.MCU_ID==0x3316)
      ||(DeviceConfig_xx.MCU_ID==0x5312)) 
      {
        if (FMReadOne(id_end+4)==1)
        {
          FdataL =Rxdata;
          if(DeviceConfig_xx.MCU_ID==0x5312)//
          {            
            if((OldIRC_M301==0xffff)&&(OldIRC_word2&0x0001))//option 1 bit15:0和option 2 bit0为1（都没被写过）
            {
              FdataL = (FdataL&0xfe) | ((OPTION_TEMPADJ&0x20)>>5);//OPTION2 温度校准位bit6
            }
            else
            {
              if(OldIRC_word2&0x0001)
              {
                FdataL |= 0x01;
              }
              else
              {
                FdataL &= 0xfe;
              }
            }        
          }
        }
        else
        {
          ERORR_VALUE=FM_Read_false;
          return 0;
        }
        if (FMReadOne(id_end+5)==1)
        {
          FdataH =Rxdata;
        }
        else
        {
          return 0;
        }              
        //reload tempadj from irc-ajd funtion
        if ((DeviceConfig_xx.MCU_ID==0x3081)||(DeviceConfig_xx.MCU_ID==0x3401))
        {
          FdataL=OPTION_FRT2;
        }
        Fdata=FdataH*0x100+FdataL;
        while(++ReCnt<6)
        {	
          if(DeviceConfig_xx.MCU_ID==0x3378||DeviceConfig_xx.MCU_ID==0x3401||DeviceConfig_xx.MCU_ID==0x3316||DeviceConfig_xx.MCU_ID==0x5312)
          {
            MC321_WriteByte(DeviceConfig_xx.OptionAddr+2,Fdata);
            for(i=0;i<3;i++)
            {
              ROMReadData=MC321_ReadByte_DiffMode(i,DeviceConfig_xx.OptionAddr+2);
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
            if(i==3)//3种模式读取都和写入的数据一致 烧写成功 7
            {
              ReCnt=0;
              break;
            }
            else if (ReCnt==4)
            {
              ReCnt=0;
              OTP_ADDR=DeviceConfig_xx.OptionAddr+2;
            //  ERORR_VALUE=OPTION_Write_false;
              if((ERORR_VALUE!=NOMALL_ERROR)&&(ERORR_VALUE!=MAGRIN1_ERROR)&&(ERORR_VALUE!=OFF_MAGRIN_ERROR))//测试用
              {             
                ERORR_VALUE=OPTION_Write_false;
              }                    
              return 0;
            }         
          }
          else
          {
            if (DeviceConfig_xx.MCU_ID==0x3081)
            {
              MC321_WriteByte(DeviceConfig_xx.OptionAddr+MCU_empty_flag+1,Fdata);
              ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+MCU_empty_flag+1);
            } 
            else
            {
              MC321_WriteByte(DeviceConfig_xx.OptionAddr+2,Fdata);
              ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+2);
            }
            if ( ROMReadData==Fdata)
            {
              ReCnt=0;
              break;
            }
            else if (ReCnt==4)
            {
              ReCnt=0;
              OTP_ADDR=DeviceConfig_xx.OptionAddr+2;
              ERORR_VALUE=OPTION_Write_false;
              return 0;
            }
          }
        }
      }
    }
    //write checksum
    while(++ReCnt<3)
    {
      //FdataH=CheckSum>>8;
      //FdataL=CheckSum;
      //MC321_WriteByte(DeviceConfig_xx.OptionAddr+0x0E,FdataH);
      if ((DeviceConfig_xx.MCU_ID!=0x3401)&&(DeviceConfig_xx.MCU_ID!=0x3316))
      //if ((DeviceConfig_xx.MCU_ID!=0x3081)&&(DeviceConfig_xx.MCU_ID!=0x3401))
      {
        if(DeviceConfig_xx.MCU_ID==0x5312)//option地址（包括预留的）0x8000-0x8007
        {
          MC321_WriteByte(DeviceConfig_xx.OptionAddr+0x07,CheckSum);
        }
        else
        {
          MC321_WriteByte(DeviceConfig_xx.OptionAddr+0x0F,CheckSum);
        }
      }
      else
      {
        if(DeviceConfig_xx.MCU_ID==0x3401)
        {
          MC321_WriteByte(0x800C,CheckSum);//目前烧录器只烧1k模式
          //ROMReadData=MC321_ReadByte(0x800C);
        }
        else if(DeviceConfig_xx.MCU_ID==0x3316)
        {
          if(CheckAddr!=0)
          {
            MC321_WriteByte(CheckAddr,CheckSum);
          }
        }
        else
        {
          break;
        }
      }
    }
    ReCnt=0;
  }
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  return 1;       
}

void MC321_MODEL_IN(u8 VddFlag)
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

  NOP;
  CLN_VDD2GND_Off;
  VPP12V_On;
  //Delay_100us(2);
  Delay_100us(5);
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
  CLN_VDD2GND_On;
  Delay_100us(5);
  //Delay_100us(2);
  OTP_SDI_1;
  NOP;
  OTP_SDI_0;
  NOP;

  CLN_VDD2GND_Off;
  VPP12V_On;
  Delay_100us(5);

  OTP_SDI_1;
  NOP;
  OTP_SDI_0;
  NOP;
  OTP_SDI_1;
  NOP;
  OTP_SDI_0;
  NOP;
  VPP12V_Off;
  CLN_VDD2GND_On;
  Delay_100us(5);
  OTP_SDI_1;
  NOP;
  OTP_SDI_0;
  Delay_1ms(1);
}

u16 MC321_PreRead(u8 command)
{
    u8 i,temp; //temp use for Nop instruce
    u16 a,read_data;
    //read 
    MC301_Instruct(command);
    NOP;
    NOP;

    read_data=0;
    for(i=0;i<16;i++)
    {
        OTP_SCK_1;  //1
        NOP;
        a=GPIO_ReadInputDataBit(OTP_SDO_PORT,OTP_SDO_PIN);
        read_data |= a<<i;

        OTP_SCK_0;//D7
        NOP;
    }

    return read_data;
}

void SetAddress(u16 OTPAddr)
{
  u8 i,temp;
  u16 a;
  MC301_Instruct(SetAddr);
  for(i=0;i<16;i++)
  {
    a =OTPAddr>>i;
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

void InputData(u16 OTPData)
{
  u8 i,temp;
  u16 a;
  
  MC301_Instruct(DataWr);
  for(i=0;i<16;i++)
  {
    a =OTPData>>i;
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
void SetOption(u16 OTPAddr,u16 OTPData)
{
//  u16 a;  
//  a=MC321_ReadByte(OTPAddr); 
//  a=MC321_ReadByte(0x8000+(OTPAddr&0x000F)); 
  SetAddress(OTPAddr);           //1．	通过0x15命令设置OPTION的映射地址
  InputData(OTPData);            //2．	通过0x16命令输入数据  
  MC301_Instruct(SetOpt);        //3．  通过0x1b命令完成OPTION修改 
  Delay_1ms(1);
//  a=MC321_ReadByte(OTPAddr); 
//  a=MC321_ReadByte(0x8000+(OTPAddr&0x000F)); 
}

u16 MC321_R()
{
	u8 i,temp; //temp use for Nop instruce
	u16 a,read_data;
	//read 
	MC301_Instruct(DataRe);
	NOP;
	NOP;

	read_data=0;
	for(i=0;i<16;i++)
	{
		OTP_SCK_1;  //1
		NOP;
       //NOP;//徐明明屏蔽
		a=GPIO_ReadInputDataBit(OTP_SDO_PORT,OTP_SDO_PIN);
		read_data |= a<<i;

		OTP_SCK_0;//D7
		NOP;
        //NOP;//徐明明屏蔽
	}

	return read_data;
}
/*******************************************************************************
* Function Name  : MC321_R_Margin.
* Description    : 不同模式读数据（不需要设置读取地址，之前设置过初始地址，读过数据，并且发了地址加1命令）
* Input          : ModeNum 0：普通模式；1：Margin-1模式；2：Margin-2模式
* Output         : read_data:读取到的数据
*******************************************************************************/
u16 MC321_R_DiffMode(u8 ModeNum)
{
  u8 i,temp; //temp use for Nop instruce
  u16 a,read_data;
  //set read mode
  MC321_SetRmode(ModeNum);

  //read 
  MC301_Instruct(DataRe);
  NOP;
  NOP;

  read_data=0;
  for(i=0;i<16;i++)
  {
    OTP_SCK_1;  //1
    NOP;
    //NOP;//徐明明屏蔽
    a=GPIO_ReadInputDataBit(OTP_SDO_PORT,OTP_SDO_PIN);
    read_data |= a<<i;

    OTP_SCK_0;//D7
    NOP;
    //NOP;//徐明明屏蔽
  }
  return read_data;
}
/*******************************************************************************
* Function Name  : MC321_SetRmode.
* Description    : 设置不同读取模式
* Input          : ModeNum 0：普通模式；1：Margin-1模式；2：Margin-2模式
* Output         : none
*******************************************************************************/
void MC321_SetRmode(u8 ReadMode)
{
  u8 i,temp; //temp use for Nop instruce
  u16 a,wData;
  if(ReadMode == 0)//普通读取
  {
    a = 0x0000;
  }
  else if(ReadMode == 1)//Margin-1模式读取
  {
    a = 0x0006;
  }
  else if(ReadMode == 2)//Off State Margin模式读取
  {
    a = 0x0005;
  }
  MC301_Instruct(SetReadMode);
  NOP;NOP;
  for(i=0;i<16;i++)
  {         
    wData =a>>i;
    if ((wData & 0x0001)==1)
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
/*
void MC321_W(u16 OTPData)
{
	u8 i,temp;
	u16 wData; 
	u32 count;

	MC301_Instruct(DataWr);
	NOP;NOP;
	for(i=0;i<16;i++)
	{
		

		wData =OTPData>>i;
		if ((wData & 0x0001)==1)
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

    if ((DeviceConfig_xx.MCU_ID==0x3401)||(DeviceConfig_xx.MCU_ID==0x3081)
        ||(DeviceConfig_xx.MCU_ID==0x3316))
    {
        wData=MC321_PreRead(PDataRe);
        if (wData!=OTPData)
        {
            return ;
        }
    }
	//program time
	MC301_Instruct(ProStr);
	for(i=0;i<8;i++)
	{
		NOP;
		OTP_SCK_1;
		NOP;
		OTP_SCK_0;	
	}
	if (DeviceConfig_xx.MCU_Type==0x02)
	{
		count=0;
		while(GPIO_ReadInputDataBit(OTP_SDO_PORT,OTP_SDO_PIN))
		{
			count++;
			//if (count>80000)
			//{
			//	break;
			//}
		}

	} 
	else
	{
		Delay_100us(DeviceConfig_xx.ProTime);
                Delay_100us(DeviceConfig_xx.ProTime);
                Delay_100us(DeviceConfig_xx.ProTime);
	}


	for(i=0;i<8;i++)
	{
		NOP;
		OTP_SCK_1;
		NOP;
		OTP_SCK_0;	
	}

}
*/
/*
void MC321_W(u16 OTPData)
{
  u8 i,temp;
  u16 wData; 
  u32 count;

  MC301_Instruct(DataWr);
  NOP;NOP;
  for(i=0;i<16;i++)
  {         
    wData =OTPData>>i;
    if ((wData & 0x0001)==1)
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
  if ((DeviceConfig_xx.MCU_ID==0x3401)||(DeviceConfig_xx.MCU_ID==0x3081)
  ||(DeviceConfig_xx.MCU_ID==0x3316))
  {
    wData=MC321_PreRead(PDataRe);
    if (wData!=OTPData)
    {
        return ;
    }
  }
  //program time
  if(DeviceConfig_xx.MCU_ID==0x3316||DeviceConfig_xx.MCU_ID==0x3378)//编程时间 90us - 110us
  {
    MC301_Instruct(ProStr);
    for(i=0;i<8;i++)
    {
      NOP;
      OTP_SCK_1;
      NOP;
      OTP_SCK_0;	
    }
    Delay_10us(9);
//    Delay_1us(10);
    for(i=0;i<8;i++)
    {
      NOP;
      OTP_SCK_1;
      NOP;
      OTP_SCK_0;	
    } 
  }
  else
  {
    for(i=0;i<8;i++)
    {
      NOP;
      OTP_SCK_1;
      NOP;
      OTP_SCK_0;	
    }
    if (DeviceConfig_xx.MCU_Type==0x02)
    {
      count=0;
      while(GPIO_ReadInputDataBit(OTP_SDO_PORT,OTP_SDO_PIN))
      {
        count++;
      }
    } 
    else
    {
      Delay_100us(DeviceConfig_xx.ProTime);
      Delay_100us(DeviceConfig_xx.ProTime);
      Delay_100us(DeviceConfig_xx.ProTime);    
    }
    for(i=0;i<8;i++)
    {
      NOP;
      OTP_SCK_1;
      NOP;
      OTP_SCK_0;	
    }
  }
}
*/
void MC321_W(u16 OTPData)
{
  u8 i,temp;
  u16 wData; 
  u32 count;

  MC301_Instruct(DataWr);
  NOP;NOP;
  for(i=0;i<16;i++)
  {         
    wData =OTPData>>i;
    if ((wData & 0x0001)==1)
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
  if (WriteConfig_xx.ReadProAddrAndData)
  {
    wData=MC321_PreRead(PDataRe);
    if (wData!=OTPData)
    {
        return ;
    }
  }
  //program time
  if(WriteConfig_xx.ProTime==1)//编程时间 90us - 110us
  {
    MC301_Instruct(ProStr);
    for(i=0;i<8;i++)
    {
      NOP;
      OTP_SCK_1;
      NOP;
      OTP_SCK_0;	
    }
    VDD_IO_On;
    Delay_10us(9);//102us
    VDD_IO_Off;
//    Delay_1us(10);
    for(i=0;i<8;i++)
    {
      NOP;
      OTP_SCK_1;
      NOP;
      OTP_SCK_0;	
    } 
  }
  else if(WriteConfig_xx.ProTime==2)//编程时间 300us
  {
    MC301_Instruct(ProStr);
    for(i=0;i<8;i++)
    {
      NOP;
      OTP_SCK_1;
      NOP;
      OTP_SCK_0;	
    }
    VDD_IO_On;
    Delay_10us(28);
    //Delay_10us(7);//102us
    //Delay_10us(7);//102us
    //Delay_10us(7);//102us
    VDD_IO_Off;
    for(i=0;i<8;i++)
    {
      NOP;
      OTP_SCK_1;
      NOP;
      OTP_SCK_0;	
    } 
  }
  else
  {
    MC301_Instruct(ProStr);
    for(i=0;i<8;i++)
    {
      NOP;
      OTP_SCK_1;
      NOP;
      OTP_SCK_0;	
    }
    if (DeviceConfig_xx.MCU_Type==0x02)
    {
      count=0;
      while(GPIO_ReadInputDataBit(OTP_SDO_PORT,OTP_SDO_PIN))
      {
        count++;
      }
    } 
    else
    {
      Delay_100us(DeviceConfig_xx.ProTime);
      Delay_100us(DeviceConfig_xx.ProTime);
      Delay_100us(DeviceConfig_xx.ProTime);    
    }
    for(i=0;i<8;i++)
    {
      NOP;
      OTP_SCK_1;
      NOP;
      OTP_SCK_0;	
    }
  }
}

/*
void MC321_W(u16 OTPData)
{
  u8 i,temp;
  u16 wData; 
  u32 count;

  MC301_Instruct(DataWr);
  NOP;NOP;
  for(i=0;i<16;i++)
  {         
    wData =OTPData>>i;
    if ((wData & 0x0001)==1)
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
  if ((DeviceConfig_xx.MCU_ID==0x3401)||(DeviceConfig_xx.MCU_ID==0x3081)
  ||(DeviceConfig_xx.MCU_ID==0x3316)||(DeviceConfig_xx.MCU_ID==0x7022)||(DeviceConfig_xx.MCU_ID==0x5222)
    ||(DeviceConfig_xx.MCU_ID==0x9033)||(DeviceConfig_xx.MCU_ID==0x5312))
  {
    wData=MC321_PreRead(PDataRe);
    if (wData!=OTPData)
    {
        return ;
    }
  }
  //program time
  if(DeviceConfig_xx.MCU_ID==0x3316||DeviceConfig_xx.MCU_ID==0x3378||DeviceConfig_xx.MCU_ID==0x6060
    ||DeviceConfig_xx.MCU_ID==0x3111||DeviceConfig_xx.MCU_ID==0x3401||DeviceConfig_xx.MCU_ID==0x5312
    ||DeviceConfig_xx.MCU_ID==0x7022||DeviceConfig_xx.MCU_ID==0x7011||DeviceConfig_xx.MCU_ID==0x7212
    ||DeviceConfig_xx.MCU_ID==0x7511||DeviceConfig_xx.MCU_ID==0x5222||DeviceConfig_xx.MCU_ID==0x7311)//编程时间 90us - 110us
  {
    MC301_Instruct(ProStr);
    for(i=0;i<8;i++)
    {
      NOP;
      OTP_SCK_1;
      NOP;
      OTP_SCK_0;	
    }
    Delay_10us(7);//102us
//    Delay_1us(10);
    for(i=0;i<8;i++)
    {
      NOP;
      OTP_SCK_1;
      NOP;
      OTP_SCK_0;	
    } 
  }
  else if(DeviceConfig_xx.MCU_ID==0x9029||DeviceConfig_xx.MCU_ID==0x9033)//编程时间 300us
  {
    MC301_Instruct(ProStr);
    for(i=0;i<8;i++)
    {
      NOP;
      OTP_SCK_1;
      NOP;
      OTP_SCK_0;	
    }
    Delay_10us(21);
    for(i=0;i<8;i++)
    {
      NOP;
      OTP_SCK_1;
      NOP;
      OTP_SCK_0;	
    } 
  }
  else
  {
    MC301_Instruct(ProStr);
    for(i=0;i<8;i++)
    {
      NOP;
      OTP_SCK_1;
      NOP;
      OTP_SCK_0;	
    }
    if (DeviceConfig_xx.MCU_Type==0x02)
    {
      count=0;
      while(GPIO_ReadInputDataBit(OTP_SDO_PORT,OTP_SDO_PIN))
      {
        count++;
      }
    } 
    else
    {
      Delay_100us(DeviceConfig_xx.ProTime);
      Delay_100us(DeviceConfig_xx.ProTime);
      Delay_100us(DeviceConfig_xx.ProTime);    
    }
    for(i=0;i<8;i++)
    {
      NOP;
      OTP_SCK_1;
      NOP;
      OTP_SCK_0;	
    }
  }
}

*/
/*******************************************************************************
* Function Name  : MC321_W_100us
* Description    : 有Margin功能的芯片烧写 时间100us
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void MC321_W_100us(u16 OTPData)
{
  u8 i,temp;
  u16 wData; 

  MC301_Instruct(DataWr);
  NOP;NOP;
  for(i=0;i<16;i++)
  {         
    wData =OTPData>>i;
    if ((wData & 0x0001)==1)
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

  //确认编程数据正确
  wData=MC321_PreRead(PDataRe);
  if (wData!=OTPData)
  {
      return ;
  }

  //program time
  MC301_Instruct(ProStr);
  for(i=0;i<8;i++)
  {
    NOP;
    OTP_SCK_1;
    NOP;
    OTP_SCK_0;	
  }
  Delay_10us(7);//102us
  for(i=0;i<8;i++)
  {
    NOP;
    OTP_SCK_1;
    NOP;
    OTP_SCK_0;	
  } 
}
void MC321_WriteByte(u16 OTPAddr,u16 OTPData)
{
	u8 i,temp;
	u16 a;
	MC301_Instruct(SetAddr);
	NOP;
	NOP;
	for(i=0;i<16;i++)
	{
		a =OTPAddr>>i;
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

	MC321_W(OTPData);
}

u16 MC321_ReadByte(u16 OTPAddr)
{
	u8 i,temp;
	u16 a;
	MC301_Instruct(SetAddr);
	for(i=0;i<16;i++)
	{
		a =OTPAddr>>i;
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

	a=MC321_R();

	return a;
}
//不同读取模式读数据
/*******************************************************************************
* Function Name  : MC321_ReadByte_DiffMode.
* Description    : 不同模式读数据（设置读取地址）
* Input          : ModeNum 0：普通模式；1：Margin-1模式；2：Margin-2模式
*                  OTPAddr：读取地址
* Output         : a读取到的数据
*******************************************************************************/

u16 MC321_ReadByte_DiffMode(u8 ModeNum,u16 OTPAddr)
{
	u8 i,temp;
	u16 a;
        
        //set read mode 
        MC321_SetRmode(ModeNum);
        //read
	MC301_Instruct(SetAddr);
	for(i=0;i<16;i++)
	{
		a =OTPAddr>>i;
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

	a=MC321_R();

	return a;
}

//--------------------------------------------------------
u8 M321IRC(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i,vdsel;
  u8 mm=0x80;
  //u8 temp;
   u16 temp0_FT,temp1_FT ;
  //power on & model in
  MC321_MODEL_IN(vdd30);
  Delay_1ms(1);
  MC301_Instruct(TestMod);

  for(i=0;i<5;i++)
  {
    MC321_IRC_INST(0X0000); //5 NOP
  }
  MC321_IRC_INST(0x3c74) ; //movai	0x80 ; //Fosc/256
  MC321_IRC_INST(0x57f9) ; //movra	0x1f9
  MC321_IRC_INST(0x3c00) ; //movai	0x00
  MC321_IRC_INST(0x57fa) ; //movra	0x1fa
  if(DeviceConfig_xx.ProVDD==0x2821) 
  {
    if (FMReadOne(Addr_Flash_Option+3)==1)
    {
      vdsel =Rxdata;
    }
    else
    {
      ERORR_VALUE=FM_Read_false;
      return 0;
    }
    MC321_IRC_INST(0x3cff) ; //movai	0xff ; //
    MC321_IRC_INST(0x57fb) ; //movra	0x1fb
    MC321_IRC_INST(0x3c00+vdsel) ; //movai	0x11
    MC321_IRC_INST(0x57fc) ; //movra	0x1fc

    MC321_IRC_INST(0x3c01) ;  //movai	0x01
    MC321_IRC_INST(0x57f8) ; 
    MC321_IRC_INST(0x0000) ;
    Delay_1ms(10);

    //temp value adj
    vdsel=0x10;
    for (i=0;i<4;i++)
    {
      MC321_IRC_INST(0x3c00+vdsel) ; //movai	0x11
      MC321_IRC_INST(0x57fc) ; //movra	0x1fc

      MC321FTtest(0xff);
      temp0_FT=IRC_VALUE;
      MC321FTtest(0x00);
      temp1_FT=IRC_VALUE;

      if((vdsel==0x1e)||(vdsel==0x02))
      {
        if ((temp0_FT>2109)&&(temp1_FT<2109))
          break;
        else if (vdsel==0x02)
        {
            vdsel=0x04;
        }
        else
            return 0;
      }

      if ((temp0_FT>2109)&&(temp1_FT<2109))
      {
         vdsel &= ~(0x10>>i);
      }
      else 
      {
         vdsel |= 0x10>>i;
      }

      vdsel |=0x08>>i;

    }
    if (vdsel!=0x04)
    {
        vdsel=vdsel+1;
    }

    MC321_IRC_INST(0x3c00+vdsel) ; //movai	0x11
    MC321_IRC_INST(0x57fc) ; //movra	0x1fc

  }  
  MC321_IRC_INST(0x3c01) ;  //movai	0x01
  MC321_IRC_INST(0x57f8) ; 
  MC321_IRC_INST(0x0000) ;
  Delay_1ms(10);
  mm=0x80;
  OPTION_FRT=0; //initial 
  for(i=0;i<8;i++)
  {
    //mm=0xff;
    //M201FTSendData(IRC_Option,IRC_Addr,mm);
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
    if (DeviceConfig_xx.ProVDD==0x2821)
    {
        OPTION_FRT=vdsel;
    }
    else
    {
        OPTION_FRT=0xff;
    }          
    OPTION_FRT2=mm;
    return 1;
  } 
  else
  {
    OPTION_FRT=0XFF;
    OPTION_FRT2=0xff;
    ERORR_VALUE=IRC_Value_false;
    return 0;
  }
}

void MC321FTtest(u8 IRC_Option)
{
	u16 temp=0x3c00;
	u16 i;
	u16 cnt[3];

    if (DeviceConfig_xx.MCU_ID==0x3081)
    {
        temp=0x0b00+IRC_Option;
        MC321_IRC_INST(temp);
        MC321_IRC_INST(0x15fb);
    }
    else if (DeviceConfig_xx.MCU_ID==0x3401)
    {
        temp=0x0400+IRC_Option;
        MC321_IRC_INST(temp);
        MC321_IRC_INST(0x00bb);
    }
    else
    {
        temp=temp+IRC_Option;
        MC321_IRC_INST(temp);
        MC321_IRC_INST(0x57fb);
    }
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
//---------------------------------------------------------
//---------------------------------------------------------
u8 MC3081IRC(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
    u8 i,opbit_L,opbit_H;
    u8 mm=0x80;
    //u8 temp;
    u16 temp0_FT,temp1_FT ;
    //power on & model in
    MC321_MODEL_IN(vdd30);


    Delay_1ms(1);

    temp1_FT=MC321_ReadByte(0XFFFF);

    MC301_Instruct(TestMod);
    
    if (DeviceConfig_xx.MCU_ID==0x3401)
    {
        CLN_VDD2GND_Off;
        VPP12V_On;  // W/R ADDR:0x38--0x39 when VPP=7.5v 
       // Delay_100us(5); 
    }
    Delay_1ms(1);

    for(i=0;i<10;i++)
    {
        MC321_IRC_INST(0X0000); //5 NOP
    }
    

    if (DeviceConfig_xx.MCU_ID==0x3401)
    {       
        MC321_IRC_INST(0x0405);              //movai           0x00    ;OPITON[12:8]=0x10   
        MC321_IRC_INST(0x00ba);              //movra           0x3a 
    
        if (FMReadOne(0x012120)==1)          //opbit addr 0x2120---0x2125, 0x2126: page index, 0x2127:timer value
        {
            opbit_L =Rxdata;
        }
        else
        {
            ERORR_VALUE=FM_Read_false;
            return 0;
        }

        MC321_IRC_INST(0x0400+opbit_L);              //movai           0x00    ;OPTION0[7:0]=0x00    
        MC321_IRC_INST(0x00b9);              //movra           0x39

        MC321_IRC_INST(0x040f);              //movai           0x00    ;addr8001[12:8]=0x00
        MC321_IRC_INST(0x00bc);              //movra           0x3c                         
        MC321_IRC_INST(0x0480);              //movai           0x80    ;addr8001[7:0]=0x80  
        MC321_IRC_INST(0x00bb);              //movra           0x3b

        if (FMReadOne(0x012120+4)==1)
        {
            opbit_L =Rxdata;
        }
        else
        {
            ERORR_VALUE=FM_Read_false;
            return 0;
        }
        if (FMReadOne(0x012120+5)==1)
        {
            opbit_H =Rxdata;
        }
        else
        {
            return 0;
        }
        
        OPTION_FRT2= opbit_L; //save return tempadj value

        MC321_IRC_INST(0x0400+opbit_H);             //movai           0x18    ;addr8002[12:8]=0x18 
        MC321_IRC_INST(0x00be);                     //movra           0x3e                         
        MC321_IRC_INST(0x0400+opbit_L);             //movai           0x0A    ;addr8002[7:0]=0x05 
        MC321_IRC_INST(0x00bd);                     //movra           0x3d 

        
        if (FMReadOne(0x012120+7)==1)  //0x2127:timer value
        {
            opbit_L =Rxdata;
        }
        else
        {
            ERORR_VALUE=FM_Read_false;
            return 0;
        }
        MC321_IRC_INST(0x0400+opbit_L);             //movai         0x01
        MC321_IRC_INST(0x0031);                     //iosw          t0cr

        MC321_IRC_INST(0x0401);                     //LHR    output from DSO 
        MC321_IRC_INST(0x00b8) ; 
        MC321_IRC_INST(0x0000) ;

    }
    else
    {   //mc30p081

        if (FMReadOne(0x012120)==1)
        {
            opbit_L =Rxdata;
        }
        else
        {
            ERORR_VALUE=FM_Read_false;
            return 0;
        }

        //OPBIT0
        MC321_IRC_INST(0X0b00+opbit_L);     //movai   0x07
        //MC321_IRC_INST(0X0b07);     //movai   0x07
        MC321_IRC_INST(0x15f9);     //movra   0x79
        MC321_IRC_INST(0X0b35);     //0x35
        MC321_IRC_INST(0x15fa);
        //OPBIT1 
        MC321_IRC_INST(0X0b80);    
        MC321_IRC_INST(0x15fb);
        MC321_IRC_INST(0X0b36);     //0x36
        MC321_IRC_INST(0x15fc);

        if (FMReadOne(0x012120+4)==1)
        {
            opbit_L =Rxdata;
        }
        else
        {
            ERORR_VALUE=FM_Read_false;
            return 0;
        }
        if (FMReadOne(0x012120+5)==1)
        {
            opbit_H =Rxdata;
        }
        else
        {
            return 0;
        }

        OPTION_FRT2=opbit_L;

        MC321_IRC_INST(0X0b00+opbit_L);     //option2
        //MC321_IRC_INST(0X0bc5);     //movai   0x07
        MC321_IRC_INST(0x15fd);
        MC321_IRC_INST(0X0b00+opbit_H);
        //MC321_IRC_INST(0X0b03);     //movai   0x07
        MC321_IRC_INST(0x15fe);

        if (FMReadOne(0x012120+7)==1)
        {
            opbit_H =Rxdata;
        }
        else
        {
            return 0;
        }
        MC321_IRC_INST(0x0b00+opbit_H);
        //MC321_IRC_INST(0X0b06);     //movai   0x07
        MC321_IRC_INST(0x15c1);   //movra   0x41 FOSC/64

        MC321_IRC_INST(0x0b01); //movai 0x01
        MC321_IRC_INST(0x15f8); //movra 0x78 //HIRC OUTPUT FROM PSDO

    }



    MC321_IRC_INST(0x0000);
    MC321_IRC_INST(0x0000) ;

    Delay_1ms(10);
    OPTION_FRT=0; //initial 
    for(i=0;i<8;i++)
    {
        //mm=0xff;
        //M201FTSendData(IRC_Option,IRC_Addr,mm);
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

    MC321FTtest(mm);

    if (DeviceConfig_xx.MCU_ID==0x3401)
    {
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
            //OPTION_FRT2--return the temp adj
            return 1;
        }

        //---------tempadj+/-1------------------------------------
        if (FMReadOne(0x012120+4)==1) //read option2 tempadj
        {
            opbit_L =Rxdata;
        }
        else
        {
            return 0;
        }

        OPTION_FRT2=opbit_L; //save temp value

        if (FMReadOne(0x012120)==1) //read option0 ,flag 16MHZ/8MHZ/4MHZ/2MHZ/1MHZ/455K
        {
            opbit_L =Rxdata;
        }
        else
        {
            return 0;
        }

        opbit_L =opbit_L>>4;
        opbit_L =opbit_L &0x07;
        if (opbit_L==2) //adj+1
        {
            OPTION_FRT2 =OPTION_FRT2+1;
        } 
        else //adj-1;
        {
            OPTION_FRT2 =OPTION_FRT2-1;
        }

        MC321_IRC_INST(0X0400+OPTION_FRT2);     //option3 L
        MC321_IRC_INST(0x00bd);

        MC321_IRC_INST(0x0000);
        MC321_IRC_INST(0x0000) ;

        Delay_1ms(10);
        mm=0x80; 
        for(i=0;i<8;i++)
        {
            //mm=0xff;
            //M201FTSendData(IRC_Option,IRC_Addr,mm);
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
            //OPTION_FRT2--return the temp adj
            return 1;
        }
        //---------tempadj_3------------------------------------
        if (FMReadOne(0x012120+4)==1) //read option2 tempadj
        {
            opbit_L =Rxdata;
        }
        else
        {
            return 0;
        }

        OPTION_FRT2=opbit_L; //save temp value

        if (FMReadOne(0x012120)==1) //read option0 ,flag 16MHZ/8MHZ/4MHZ/2MHZ/1MHZ/455K
        {
            opbit_L =Rxdata;
        }
        else
        {
            return 0;
        }

        opbit_L =opbit_L>>4;
        opbit_L =opbit_L &0x07;
        
        if(opbit_L==0||opbit_L==1)
        {
          OPTION_FRT2 =OPTION_FRT2+1;
        }
        else if (opbit_L==2)
        {
          OPTION_FRT2 =OPTION_FRT2-1;
        }
        else
        {
          OPTION_FRT2 =OPTION_FRT2-2;
        }

        MC321_IRC_INST(0X0400+OPTION_FRT2);     //option3 L
        MC321_IRC_INST(0x00bd);

        MC321_IRC_INST(0x0000);
        MC321_IRC_INST(0x0000) ;

        Delay_1ms(10);
        mm=0x80; 
        for(i=0;i<8;i++)
        {
            //mm=0xff;
            //M201FTSendData(IRC_Option,IRC_Addr,mm);
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
            //OPTION_FRT2--return the temp adj
            return 1;
        }
        else
        {
            OPTION_FRT=0XFF;
            OPTION_FRT2=0xff;
            return 0;
        }

    }
    else if (DeviceConfig_xx.MCU_ID==0x3081)
    {
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
            //OPTION_FRT2--return the temp adj
            return 1;
        }

        //---------tempadj+/-1------------------------------------
        if (FMReadOne(0x012120+4)==1) //read option2 tempadj
        {
            opbit_L =Rxdata;
        }
        else
        {
            return 0;
        }

        OPTION_FRT2=opbit_L; //save temp value

        if (FMReadOne(0x012120)==1) //read option0 ,flag 16MHZ/8MHZ/4MHZ/2MHZ/1MHZ/455K
        {
            opbit_L =Rxdata;
        }
        else
        {
            return 0;
        }

        opbit_L =opbit_L>>4;
        opbit_L =opbit_L &0x07;
        if ((opbit_L==5)||(opbit_L==2)) //adj+1
        {
            OPTION_FRT2 =OPTION_FRT2+1;
        } 
        else //adj-1;
        {
            OPTION_FRT2 =OPTION_FRT2-1;
        }
        
        MC321_IRC_INST(0X0b00+OPTION_FRT2);     //option2
        //MC321_IRC_INST(0X0bc5);     //movai   0x07
        MC321_IRC_INST(0x15fd);

        MC321_IRC_INST(0x0000);
        MC321_IRC_INST(0x0000) ;

        Delay_1ms(10);
        mm=0x80; 
        for(i=0;i<8;i++)
        {
            //mm=0xff;
            //M201FTSendData(IRC_Option,IRC_Addr,mm);
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
            //OPTION_FRT2--return the temp adj
            return 1;
        }
        //---------tempadj_3------------------------------------
        if (FMReadOne(0x012120+4)==1) //read option2 tempadj
        {
            opbit_L =Rxdata;
        }
        else
        {
            return 0;
        }

        OPTION_FRT2=opbit_L; //save temp value

        if (FMReadOne(0x012120)==1) //read option0 ,flag 16MHZ/8MHZ/4MHZ/2MHZ/1MHZ/455K
        {
            opbit_L =Rxdata;
        }
        else
        {
            return 0;
        }

        opbit_L =opbit_L>>4;
        opbit_L =opbit_L &0x07;
//        if ((opbit_L==5)||(opbit_L==2)) //adj+1
//        {
//            OPTION_FRT2 =OPTION_FRT2+1;
//        } 
//        else //adj-1;
//        {
//            OPTION_FRT2 =OPTION_FRT2-1;
//        }
        if ((opbit_L==0)||(opbit_L==1)||(opbit_L==4)) //adj+1
        {
            OPTION_FRT2 =OPTION_FRT2+1;
        } 
        else if((opbit_L==2)||(opbit_L==5)) //adj-1;
        {
            OPTION_FRT2 =OPTION_FRT2-1;
        }        
        else
        {
          OPTION_FRT2 =OPTION_FRT2-2;
        }
        MC321_IRC_INST(0X0b00+OPTION_FRT2);     //option2
        //MC321_IRC_INST(0X0bc5);     //movai   0x07
        MC321_IRC_INST(0x15fd);

        MC321_IRC_INST(0x0000);
        MC321_IRC_INST(0x0000) ;

        Delay_1ms(10);
        mm=0x80; 
        for(i=0;i<8;i++)
        {
            //mm=0xff;
            //M201FTSendData(IRC_Option,IRC_Addr,mm);
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
            //OPTION_FRT2--return the temp adj
            return 1;
        }
        else
        {
            OPTION_FRT=0XFF;
            OPTION_FRT2=0xff;
            return 0;
        }

    }

    OPTION_FRT=0XFF;
    OPTION_FRT2=0xff;
    return 0;


}

//------------------------------------------------------------------

u8 M9029IRC(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
	u8 i,optf2, optfa;
	u16 mm,temp0_FT,temp1_FT;
//	u8 tempadj;

	mm=0x0400; // & (rdv<<8);
	//power on & model in 
	MC321_MODEL_IN(vdd30);
	Delay_1ms(1);
	MC301_Instruct(TestMod);

	if (FMReadOne(0x0101E4)==1)
	{
		optf2 =Rxdata;
	}
	else
	{
		ERORR_VALUE=FM_Read_false;
		return 0;
	}

	if (FMReadOne(0x0101F4)==1)
	{
		optfa =Rxdata;
	}
	else
	{
		ERORR_VALUE=FM_Read_false;
		return 0;
	} 

	for(i=0;i<5;i++)
	{
		MC321_IRC_INST(0X0000); //5 NOP
	}
	
	MC321_IRC_INST(0xff01) ; //HIRC ouput mode
	
	temp0_FT=0xf200 +optf2;
	MC321_IRC_INST(temp0_FT) ; //VDSEL=1
	temp0_FT=0xfa00+optfa;
	MC321_IRC_INST(temp0_FT);

        //-------------TEMPADJ=0X02 -----------------------------------------------------
	OPTION_FRT=0xff; //initial
	OPTION_FRT2=0xff;
	for(i=0;i<11;i++)
	{
		//mm=0xff;
		//M201FTSendData(IRC_Option,IRC_Addr,mm);
		M9029FTtest(mm,0x10);
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

	M9029FTtest(mm+1,0x10);
	temp0_FT=IRC_VALUE;

	M9029FTtest(mm,0x10);
	temp1_FT=IRC_VALUE;

	if((temp0_FT-IRC_FreqType)<(IRC_FreqType-temp1_FT))
	{
		mm=mm+1;
		IRC_VALUE=temp0_FT;
	}

	if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
	{
		OPTION_FRT=mm/256;
		OPTION_FRT =OPTION_FRT | 0x10;
		OPTION_FRT2=mm%256;
		return 1;
	}

	//-------------TEMPADJ=0X03 -----------------------------------------------------
	OPTION_FRT=0xff; //initial
	OPTION_FRT2=0xff;
        mm=0x400;
	for(i=0;i<11;i++)
	{
		//mm=0xff;
		//M201FTSendData(IRC_Option,IRC_Addr,mm);
		M9029FTtest(mm,0x18);
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

	M9029FTtest(mm+1,0x18);
	temp0_FT=IRC_VALUE;

	M9029FTtest(mm,0x18);
	temp1_FT=IRC_VALUE;

	if((temp0_FT-IRC_FreqType)<(IRC_FreqType-temp1_FT))
	{
		mm=mm+1;
		IRC_VALUE=temp0_FT;
	}

	if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
	{
		OPTION_FRT=mm/256;
		OPTION_FRT =OPTION_FRT | 0x18;
		OPTION_FRT2=mm%256;
		return 1;
	}
	//-------------TEMPADJ=0X01 -----------------------------------------------------
        OPTION_FRT=0xff; //initial
        OPTION_FRT2=0xff;
//	tempadj=0x11;
        mm=0x400;
	for(i=0;i<11;i++)
	{
		//mm=0xff;
		//M201FTSendData(IRC_Option,IRC_Addr,mm);
		M9029FTtest(mm,0x08);
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

	M9029FTtest(mm+1,0x08);
	temp0_FT=IRC_VALUE;

	M9029FTtest(mm,0x08);
	temp1_FT=IRC_VALUE;

	if((temp0_FT-IRC_FreqType)<(IRC_FreqType-temp1_FT))
	{
		mm=mm+1;
		IRC_VALUE=temp0_FT;
	}

	if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
	{
		OPTION_FRT=mm/256;
		OPTION_FRT =OPTION_FRT | 0x08;
		OPTION_FRT2=mm%256;
		return 1;
	}

		OPTION_FRT=0xff;
		OPTION_FRT2=0xff;
		return 0;
}

u8 M9029IRC_last(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i,optf2, optfa;
  u16 mm,temp0_FT;

  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  ConfigTIM4();
  Delay_1ms(5);
  //power on & model in 
  MC321_MODEL_IN(vdd30);
  Delay_1ms(1);
  MC301_Instruct(TestMod);

  if (FMReadOne(0x0101E4)==1)
  {
    optf2 =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    return 0;
  }

  if (FMReadOne(0x0101F4)==1)
  {
    optfa =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    return 0;
  } 

  for(i=0;i<5;i++)
  {
          MC321_IRC_INST(0X0000); //5 NOP
  }
	
  MC321_IRC_INST(0xff01) ; //HIRC ouput mode
  
  temp0_FT=0xf200 +optf2;
  MC321_IRC_INST(temp0_FT) ; //VDSEL=1
  temp0_FT=0xfa00+optfa;
  MC321_IRC_INST(temp0_FT);

  mm=OPTION_FRT*256+OPTION_FRT2;
  mm &=0x7ff;
  optfa=OPTION_FRT&0xf8;
  M9029FTtest(mm,optfa);
   
  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    return 1;
  }
  OPTION_FRT=0xff;
  OPTION_FRT2=0xff;
  return 0;
}
 void M9029FTtest(u16 irc_adj_value,u8 tempadj)
 {
	 u16 temp=0xf000;
	 u8  adjH,adjL,optf1;
	 u16 i;
	 u16 cnt[3];

	 adjH=irc_adj_value/256; //H
	 adjL=irc_adj_value%256;

	 if (FMReadOne(0x0101E2)==1)
	 {
		 optf1 =Rxdata;
	 }


	 temp=0xf000+adjL;
	 // temp=0x3c00+i;
	 MC321_IRC_INST(temp);
	 //MC321_IRC_INST(0x57fb);
	 optf1=optf1 & 0x80;
	 optf1=optf1 | tempadj;
	 optf1=optf1 | adjH;
	 temp=0xf100+optf1;
	 MC321_IRC_INST(temp);
	// MC321_IRC_INST(0x57fc);
	// MC321_IRC_INST(0x0000);

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
//-----------------------------------------------------------------

u8 MC32P5312IRC_11Bit(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i;
  u8 optL,optH,opt_2,Fdata_H;
  u16 mm,Fdata,temp0_FT,temp1_FT;
  u8 OPTION_FAS;
  u8 OPTION_TEMPADJ_1,OPTION_TEMPADJ_2,OPTION_TEMPADJ_3;
  
  FM_CS_0;
  mm=0x400 ;//& (rdv<<8);
  //power on & model in
//  MC321_MODEL_IN(vdd30);
  Delay_1ms(1);
  //说明：上位机会根据用户选的FAS(内部高频振荡器频率选择位)计算处理得出FDS、RCSMTB、VSDEL相关位
  //option 0保持默认值
  //option 2中的FAS、FDS、RCSMTB、VDSEL取上位机发到flash中的值 
  
  //{先设置测试条件，即写FAS、FDS、RCSMTB、VDSEL相关位
  //同时确定两次校验要用的温度校准值OPTION_TEMPADJ_1、OPTION_TEMPADJ_2及不同FAS对应IRC_FreqMin、IRC_FreqMax、IRC_FreqType
  if (FMReadOne(Addr_Flash_OptionProtec+4)==1)//option 2 低字节
  {
    optL =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }
  if (FMReadOne(Addr_Flash_OptionProtec+5)==1)//option 2 高字节
  {
    optH =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }    
  
  opt_2 = optL;
  OPTION_FAS = optH & 0x07; //确定是16M 8M 4M 2M 1M 455K哪一种，再确定两次校验要用的温度校准值OPTION_TEMPADJ_1、OPTION_TEMPADJ_2
  
  Fdata=MC321_ReadByte(0X8002);//读option 2 看芯片是否被写过  
  if(Fdata != 0xffff)//芯片如果被写过 需要先读 判断这次用的FAS和上次用的是否一致 不一致 直接报错
  {
    Fdata_H = Fdata/256;
    Fdata_H &= 0x07;
    if(OPTION_FAS != Fdata_H)//选用的内部高频震荡器频率不一致 
    {
      ERORR_VALUE=HIRC_DIFF;//直接报错
      FM_CS_1;
      return 0;
    }
  }  
  MC301_Instruct(TestMod);

  for(i=0;i<5;i++)
  {
    MC321_IRC_INST(0X0000); //5 NOP
  }
  if(OPTION_FAS == 0)//16M
  {
    OPTION_TEMPADJ_1 = 0x0a;
    OPTION_TEMPADJ_2 = 0x09;
    OPTION_TEMPADJ_3 = 0x0b;
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;
    optL = 0x70;
  }
  else if(OPTION_FAS == 1)//8M
  {
    OPTION_TEMPADJ_1 = 0x12;
    OPTION_TEMPADJ_2 = 0x11;
    OPTION_TEMPADJ_3 = 0x13;
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;   
    optL = 0x60;
  }
  else if(OPTION_FAS == 2)//4M
  {
    OPTION_TEMPADJ_1 = 0x29;
    OPTION_TEMPADJ_2 = 0x2a;
    OPTION_TEMPADJ_3 = 0x28;
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
    optL = 0x50;
  }
  else if(OPTION_FAS == 3)//2M
  {
    OPTION_TEMPADJ_1 = 0x3F;
    OPTION_TEMPADJ_2 = 0x3E; 
    OPTION_TEMPADJ_3 = 0x3D;
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
    optL = 0x40;
  }   
  else if(OPTION_FAS == 4)//1M
  {
    OPTION_TEMPADJ_1 = 0x3F;
    OPTION_TEMPADJ_2 = 0x3E; 
    OPTION_TEMPADJ_3 = 0x3D;
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
    optL = 0x30;
  }   
  else if(OPTION_FAS == 5)//455K
  {
    OPTION_TEMPADJ_1 = 0x3F;
    OPTION_TEMPADJ_2 = 0x3E; 
    OPTION_TEMPADJ_3 = 0x3D;
    IRC_FreqType = 2275;
    IRC_FreqMin = 2268;
    IRC_FreqMax = 2282;    
    optL = 0x10;
  }   
  
  mm=0x3c00+optL;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57f9);     //opt0 L
  
  optH = (optH & 0x3f) | 0xd8;//opt2 FAS FDS RCSMTB相关位 注意：校验时 要求FDS要写入固定值11
  mm=0x3c00+optH;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fe);     //opt2 H
  
  optL = (opt_2 & 0x02) | 0xfd;//VSDEL相关位
  mm=0x3c00+optL;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fd);     //opt2 L
  
  OPTION_TEMP = optH;
  OPTION_TEMP = (OPTION_TEMP<<8)&0xff00;
  OPTION_TEMP = OPTION_TEMP+optL;
//  Fdata = optH<<8 + optL;
  
  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;

  MC321_IRC_INST(0x3c01);     //movai 0x01
  MC321_IRC_INST(0x57f8);     //movra 0x1f8 //set HIRC output to SDO

  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;   
  //}
  //{开始校验
  //{先用OPTION_TEMPADJ_1进行第一次校验
  mm=0x400;//频率校准位先给中间值
  OPTION_FRT=0; //initial 
  OPTION_FRT2=0; 
  IRC_MODEL=0;
  for(i=0;i<11;i++)
  {
    MC32P5312FTtest_11Bit(mm,OPTION_TEMPADJ_1);
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
  MC32P5312FTtest_11Bit(mm+1,OPTION_TEMPADJ_1);
  temp0_FT=IRC_VALUE;  
  MC32P5312FTtest_11Bit(mm,OPTION_TEMPADJ_1);
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
    OPTION_TEMPADJ = OPTION_TEMPADJ_1;
    return 1;//第一次校验成功，结束校验
  }
  //}第一次校验失败
  //{第一次校验失败，再用OPTION_TEMPADJ_1进行第二次校验   
  mm=0x400;
  OPTION_FRT=0; //initial 
  OPTION_FRT2=0;
  IRC_MODEL=1;
  for(i=0;i<11;i++)
  {
    MC32P5312FTtest_11Bit(mm,OPTION_TEMPADJ_2);
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
  MC32P5312FTtest_11Bit(mm+1,OPTION_TEMPADJ_2);
  temp0_FT=IRC_VALUE;

  MC32P5312FTtest_11Bit(mm,OPTION_TEMPADJ_2);
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
    OPTION_TEMPADJ = OPTION_TEMPADJ_2;   
    return 1;//第二次校验成功，结束校验
  } 
  //}第二次校验失败

  //{第二次校验失败，再用OPTION_TEMPADJ_3进行第三次校验   
  mm=0x400;
  OPTION_FRT=0; //initial 
  OPTION_FRT2=0;
  IRC_MODEL=2;
  for(i=0;i<11;i++)
  {
    MC32P5312FTtest_11Bit(mm,OPTION_TEMPADJ_3);
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
  MC32P5312FTtest_11Bit(mm+1,OPTION_TEMPADJ_3);
  temp0_FT=IRC_VALUE;

  MC32P5312FTtest_11Bit(mm,OPTION_TEMPADJ_3);
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
    OPTION_TEMPADJ = OPTION_TEMPADJ_3;   
    return 1;//第二次校验成功，结束校验
  } 
  //}第二次校验失败
 
  OPTION_FRT=0xff;
  OPTION_FRT2=0xff;
  OPTION_TEMPADJ = 0xff;  
  return 0;
  //}校验结束
}
void MC31P11FTtest_11Bit(u16 irc_adj_value,u8 TempADJ)
{
	u16 temp=0x0400;
	u8  adjH,adjL;
	u16 i;
	u16 cnt[3];

	adjH=irc_adj_value/256; //H
	adjL=irc_adj_value%256;
    
    if (DeviceConfig_xx.MCU_ID==0x3316)
    {
        //if (FMReadOne(addr_Flash_OptionProtec16K+7)==1)
        //{
        //    TempADJ =Rxdata;
        //}
        //else
        //{
        //    ERORR_VALUE=FM_Read_false;
        //   // return 0;
        //}

        TempADJ=TempADJ & 0xf8;
        adjH=adjH +TempADJ;

        temp=0x3c00+adjL;
        MC321_IRC_INST(temp);
        MC321_IRC_INST(0x57fc); //option2 L
        
        temp=0x3c00+adjH;
        MC321_IRC_INST(temp); //option2
        MC321_IRC_INST(0x57fd); //option2 h
        

    }
    else
    {
        temp=0x0400+adjL;
        // temp=0x3c00+i;
        MC321_IRC_INST(temp);
        MC321_IRC_INST(0x0afb);
        temp=0x0400+adjH;
        MC321_IRC_INST(temp);
        MC321_IRC_INST(0x0afc);
    }

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
//5312
//OSCCAL是11位 TADJ是6位
void MC32P5312FTtest_11Bit(u16 irc_adj_value,u8 TempADJ)
{
  u16 temp=0x0400;
  u8  adjH,adjL;
  u16 i;
  u16 cnt[3];
  u16   Fdata;
  u8 j;
  //OPTION 1：
  //bit15:11 --  bit5:bit0 TADJ
  //bit10:0  --  bit10:0   OSCCAL
  
  Fdata = ((TempADJ & 0x001f)<<11) + (irc_adj_value & 0x07ff);
  adjH=(Fdata&0xff00)>>8; //H
  adjL=Fdata&0x00ff; //L  
  
  temp=0x3c00+adjL;
  MC321_IRC_INST(temp);
  MC321_IRC_INST(0x57fb); //option1 L
  MC321_IRC_INST(0x0000);

  temp=0x3c00+adjH;
  MC321_IRC_INST(temp); //
  MC321_IRC_INST(0x57fc); //option1 H  
  MC321_IRC_INST(0x0000);
  
  //OPTION 2：bit0 -- bit6 TADJ 
  //特别说明；OPTION 2中其他位不能在此处改 
  
//  Fdata = (TempADJ & 0x0040)>>6 | (OPTION_TEMP&0xfffe);
  Fdata = (TempADJ & 0x0020)>>5 | (OPTION_TEMP&0xfffe);
  adjH=(Fdata&0xff00)>>8; //H
  adjL=Fdata&0x00ff; //L  
  
  temp=0x3c00+adjL;
  MC321_IRC_INST(temp);
  for(j=0;j<10;j++)
  {
    j=j;
  }
  MC321_IRC_INST(0x57fd); //option2 L
  MC321_IRC_INST(0x0000);

  temp=0x3c00+adjH;
  MC321_IRC_INST(temp); //
  MC321_IRC_INST(0x57fe); //option2 H  

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
//----------------------------------------------------------------------

u8 M321IRC_11Bit(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i;
  //	u8 rdv;
  u16 mm,temp0_FT,temp1_FT;

  if (FMReadOne(Addr_Flash_OptionProtec8k+3)==1)
  {
  //	rdv =Rxdata;
  }
  else
  {
          ERORR_VALUE=FM_Read_false;
          return 0;
  } 


  //power on & model in
  MC321_MODEL_IN(vdd30);
  Delay_1ms(1);
  MC301_Instruct(TestMod);

  for(i=0;i<5;i++)
  {
          MC321_IRC_INST(0X0000); //5 NOP
  }
  MC321_IRC_INST(0x3c70) ; //movai	0x80 ; //Fosc/128,FCPUS=111
  MC321_IRC_INST(0x57f9) ; //movra	0x1f9
  MC321_IRC_INST(0x3c1c) ; //movai	0x1c
  MC321_IRC_INST(0x57fa) ; //movra	0x1fa

  MC321_IRC_INST(0x3c00) ; //movai	0x80 ; //Fosc/128
  MC321_IRC_INST(0x57fb) ; //movra	0x1fb
  MC321_IRC_INST(0x3cfc) ; //movai	0x00 
  MC321_IRC_INST(0x57fc) ; //movra	0x1fc

  MC321_IRC_INST(0x3c07) ; //movai	0x07 ; 
  MC321_IRC_INST(0x57fd) ; //movra	0x1fd

  MC321_IRC_INST(0x3c01) ;  //movai	0x01
  MC321_IRC_INST(0x57f8) ; 
  MC321_IRC_INST(0x0000) ;

  //--------TEMPADJ=04H ------------------------------------------------------------
  OPTION_FRT=0; //initial
  //mm=0xfc00 & (rdv<<8);
  IRC_MODEL=0;
  mm=0x2400; //atemp=0x04
  for(i=0;i<11;i++)
  {
          //mm=0xff;
          //M201FTSendData(IRC_Option,IRC_Addr,mm);
          MC321FTtest_11Bit(mm);
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

  MC321FTtest_11Bit(mm+1);
  temp0_FT=IRC_VALUE;

  MC321FTtest_11Bit(mm);
  temp1_FT=IRC_VALUE;

  if((temp0_FT-IRC_FreqType)<(IRC_FreqType-temp1_FT))
  {
  mm=mm+1;
  IRC_VALUE=temp0_FT;
  }

  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
  OPTION_FRT=mm/256;  // hight byte?
  OPTION_FRT2=mm%256;
  return 1;
  } 

  //--------TEMPADJ=05H ------------------------------------------------------------

  OPTION_FRT=0; //initial
  //mm=0xfc00 & (rdv<<8);
  IRC_MODEL=1;
  mm=0x2c00; //atemp=0x04
  for(i=0;i<11;i++)
  {
  //mm=0xff;
  //M201FTSendData(IRC_Option,IRC_Addr,mm);
  MC321FTtest_11Bit(mm);
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

  MC321FTtest_11Bit(mm+1);
  temp0_FT=IRC_VALUE;

  MC321FTtest_11Bit(mm);
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
//--------TEMPADJ=03H ------------------------------------------------------------

  if(DeviceConfig_xx.MCU_ID==0x3378)
  {
    OPTION_FRT=0; //initial
    //mm=0xfc00 & (rdv<<8);
    IRC_MODEL=2;
    mm=0x1c00; //atemp=0x03
    for(i=0;i<11;i++)
    {
        //mm=0xff;
        //M201FTSendData(IRC_Option,IRC_Addr,mm);
        MC321FTtest_11Bit(mm);
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

    MC321FTtest_11Bit(mm+1);
    temp0_FT=IRC_VALUE;

    MC321FTtest_11Bit(mm);
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
  }
  OPTION_FRT=0xff;
  OPTION_FRT2=0xff;
  return 0; 
}

u8 M321IRC_11Bit_last(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i;
  u16 mm;
  
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  ConfigTIM4();
  Delay_1ms(5);

  //power on & model in
  MC321_MODEL_IN(vdd30);
  Delay_1ms(1);
  MC301_Instruct(TestMod);

  for(i=0;i<5;i++)
  {
    MC321_IRC_INST(0X0000); //5 NOP
  }
  MC321_IRC_INST(0x3c70) ; //movai	0x80 ; //Fosc/128,FCPUS=111
  MC321_IRC_INST(0x57f9) ; //movra	0x1f9
  MC321_IRC_INST(0x3c1c) ; //movai	0x1c
  MC321_IRC_INST(0x57fa) ; //movra	0x1fa

  MC321_IRC_INST(0x3c00) ; //movai	0x80 ; //Fosc/128
  MC321_IRC_INST(0x57fb) ; //movra	0x1fb
  MC321_IRC_INST(0x3cfc) ; //movai	0x00 
  MC321_IRC_INST(0x57fc) ; //movra	0x1fc

  MC321_IRC_INST(0x3c07) ; //movai	0x07 ; 
  MC321_IRC_INST(0x57fd) ; //movra	0x1fd

  MC321_IRC_INST(0x3c01) ;  //movai	0x01
  MC321_IRC_INST(0x57f8) ; 
  MC321_IRC_INST(0x0000) ;

  mm=OPTION_FRT*256+OPTION_FRT2; 
  MC321FTtest_11Bit(mm);
  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    return 1;
  } 
  else
  {
    OPTION_FRT=0xff;
    OPTION_FRT2=0xff;
    return 0;
   }
}

void MC321FTtest_11Bit(u16 irc_adj_value)
{
	u16 temp=0x3c00;
	u8  adjH,adjL;
	u16 i;
	u16 cnt[3];
	
	  adjH=irc_adj_value/256; //H
	  adjL=irc_adj_value%256;

	  temp=0x3c00+adjL;
         // temp=0x3c00+i;
	  MC321_IRC_INST(temp);
	  MC321_IRC_INST(0x57fb);
	  temp=0x3c00+adjH;
      MC321_IRC_INST(temp);
	  MC321_IRC_INST(0x57fc);
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

void MC321_IRC_INST(u16 OTPData)
{
  u8 i,temp;
  u16 a; 

  for(i=0;i<16;i++)
  {
    a =OTPData>>i;
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

  //2 CLK Q1,Q2
  OTP_SCK_1;
  NOP;
  OTP_SCK_0; //Q1
  NOP;
  OTP_SCK_1;
  NOP;
  OTP_SCK_0;//Q2
  NOP;
}