

#include "stm32f10x.h"
#include "global.h"
#include "erorrNum.h"
#include "spi.h"
#include "OTPRW.h"
#include "power.h"
#include "IRC.h"
#include "M201IRC.h"
#include "MC224BPro.h"
#include "Config.h"
#include "ADC.h"



//u8 MC224B_Program()
//{
//  //u16 i;
//  u8  OldIRC=0;
//  u8  ReCnt=0;
//  u8  Fdata;
//  u8  ReFlag=0;
//  u16 CheckSum=0;
//  u8 id_length=0;
//  u16 id_addr0,id_addr1,id_addr2,id_addr3;
//  u32 id_end,current_id,max_id;
////  //IRC adjust
////  if (DeviceConfig_xx.IRC_Select==0x01)
////  {
////    ReFlag=M201IRC(0x0118,DeviceConfig_xx.IRC_OPTION,0x1c05,DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
////    if(ReFlag == 0)
////    {
////      return 0;
////    }
////  }
//
//  //power on
//  if(OTP_Mode_IN(DeviceConfig_xx.MCU_ID,vpp12,vdd65)==0)	
//  {
//    ERORR_VALUE=OTP_ModeIn_false;
//    return 0;
//  }
//
//  if (TestAdc()==0)
//  {
//    return 0;
//  }
////Write IRC option
//  if (DeviceConfig_xx.OptionSize >= 2)
//  {
//    if (DeviceConfig_xx.IRC_Select==0x02)
//    {
//      OPTION_FRT =OTP_ReadByte(DeviceConfig_xx.IRC_VALUE_ADDR);
//      if ((OPTION_FRT == 0xff)||(OPTION_FRT==0x00))
//      {
//        OPTION_FRT =OTP_ReadByte(DeviceConfig_xx.IRC_VALUE_ADDR-2);
//        if ((OPTION_FRT == 0xff)||(OPTION_FRT==0x00))
//        {
//          ERORR_VALUE=IRC_Value_0XFF;
//          return 0;
//        }
//      }
//    }
//
//    OldIRC =OTP_ReadByte(0x0001);
//    if (OldIRC == 0xff)
//    {
//      //write IRC
//      if ((DeviceConfig_xx.MCU_ID ==0x0111 )||(DeviceConfig_xx.MCU_ID ==0x0101) )
//      {
//        OPTION_FRT = OPTION_FRT + DeviceConfig_xx.IRC_Shift_Value;
//      }
//      while(++ReCnt<6)
//      {		
//        OTP_WriteByte(0x0001,OPTION_FRT);
//        if (OTP_ReadByte(0x0001) == OPTION_FRT)
//        {
//          ReCnt=0;
//          break;
//        }
//        else if (ReCnt==4)
//        {
//          ERORR_VALUE=IRC_Write_false;
//          return 0;
//        }
//       }            
//     }
//  }
//
////Write ROM
//  if(DeviceConfig_xx.WriteEn & 0x0f)
//  { //temp add code
//    CheckSum=0;
//    FM_CS_0;
//    FMReadMore(Addr_Flash_ROMStart+DeviceConfig_xx.RomFirAddr-1);
//    Fdata =Rxdata;
//    
//    id_length=iic_data[1];
//    id_addr0=(iic_data[2]<<8)+iic_data[3];// ADDRL
//    id_addr1=(iic_data[4]<<8)+iic_data[5];
//    id_addr2=(iic_data[6]<<8)+iic_data[7];
//    id_addr3=(iic_data[8]<<8)+iic_data[9];//ADDRH
//    current_id=(iic_data[10]<<24)+(iic_data[11]<<16)+(iic_data[12]<<8)+iic_data[13];
//    id_end=(iic_data[15]<<24)+(iic_data[16]<<16)+(iic_data[17]<<8)+iic_data[18];
//    max_id=(iic_data[27]<<24)+(iic_data[28]<<16)+(iic_data[29]<<8)+iic_data[30]; //max-id
//
//    if (current_id>max_id)
//    {
//      iic_data[10]=iic_data[23];
//      iic_data[11]=iic_data[24];
//      iic_data[12]=iic_data[25];
//      iic_data[13]=iic_data[26];
//    }
//    if (iic_data[0]==0x0a)
//    {
//      if (OKcounter>=id_end)
//      {
//        ERORR_VALUE=OK_Counter_Full;
//        return 0; // full limit
//      }
//    }
//
//    for(OTP_ADDR=DeviceConfig_xx.RomFirAddr;OTP_ADDR<=DeviceConfig_xx.RomEndAddr;OTP_ADDR++)
//    {
//      SPI_ReadByte();
//      Fdata=Rxdata;
//      CheckSum +=Fdata;
//
//      //2012.09.12 dynamic id
//      if (iic_data[0]==0x0a) //id enable
//      {
//        if (OTP_ADDR==id_addr0)
//        {
//          Fdata=iic_data[13];
//        }
//        else if ((OTP_ADDR==id_addr1)&& (id_length>1))
//        {
//          Fdata=iic_data[12];
//        }
//        else if ((OTP_ADDR==id_addr2)&& (id_length>2))
//        {
//          Fdata=iic_data[11];
//        }
//        else if ((OTP_ADDR==id_addr3)&&(id_length>3))
//        {
//          Fdata=iic_data[10];
//        }
//      }
//      Rxdata=Fdata;
//
//      if(Fdata ==0xff)
//      {
//        OTP_ReadData=OTP_ReadByte(OTP_ADDR);
//        if ( OTP_ReadData != 0xff)
//        {
//          ERORR_VALUE=ROM_Write_false;
//          return 0;
//        }
//        continue;
//      }
//      while(++ReCnt<6)
//      {		
//        OTP_WriteByte(OTP_ADDR,Fdata);
//        OTP_ReadData=OTP_ReadByte(OTP_ADDR);
//        if (OTP_ReadData ==Fdata)
//        {
//          ReCnt=0;
//          break;
//        }
//        else if (ReCnt==4)
//        {
//          ERORR_VALUE=ROM_Write_false;
//          return 0;
//        }
//      }
//    }
//    FM_CS_1;
//    //Write checkSum
//    Fdata =CheckSum/0x100;
//    OldIRC=CheckSum%0x100;
//    while(++ReCnt<6)
//    {		
//      OTP_WriteByte(0x0010,Fdata);
//      OTP_WriteByte(0x0011,OldIRC);
//      OTP_ReadData=OTP_ReadByte(0x0010);
//      ReFlag =OTP_ReadByte(0x0011);
//      if ((OTP_ReadData ==Fdata ) && (ReFlag==OldIRC))
//      {
//        ReCnt=0;
//        break;
//      }
//      else if (ReCnt==4)
//      {
//        ReCnt=0;
//        break;
//      }
//    }
//        
//   }//Write ROM 
////Write Option
//  if(DeviceConfig_xx.WriteEn & 0xf0)
//  {
//    if (FMReadOne(Addr_Flash_OptionProtec)==1)
//    {
//      Fdata =Rxdata;
//    }
//    else
//    {
//      ERORR_VALUE=FM_Read_false;
//      return 0;
//    }
//    while(++ReCnt<6)
//    {		
//      OTP_WriteByte(DeviceConfig_xx.OptionAddr,Fdata);
//      if (OTP_ReadByte(DeviceConfig_xx.OptionAddr) ==Fdata)
//      {
//        ReCnt=0;
//        break;
//      }
//      else if (ReCnt==4)
//      {
//        //POWER_OFF(vpp00,vdd00);
//        OTP_ADDR=DeviceConfig_xx.OptionAddr;
//        ERORR_VALUE=OPTION_Write_false;
//        return 0;
//      }
//    }
//  }
//  POWER_OFF(vpp00,vdd00);
//  if (ReCnt>=5)
//  {
//    ERORR_VALUE=Program_Erorr;
//    return 0;
//  }
//  Delay_1ms(200);
//  return 1;     
//}
//
u8 MC20P02B_Program()
{
  u8  i,j;
  u8  ReCnt=0;
  u8  Fdata;
  u8  ReFlag=0;
  u16 CheckSum=0;
  u8 id_length=0;
  u16 id_addr0,id_addr1,id_addr2,id_addr3;
  u32 id_end,current_id,max_id;

  u8    ForceReCnt=0;  
  u8    SuccessCnt;   
  u8    op_addr=0;
  u8    option_shift=0;  
  u16   Addres;
  u8  OldIRC=0;
  
  WriteConfig_xx.RomBit=0x3fff;
  WriteConfig_xx.ProTime=1;
  WriteConfig_xx.ReadProAddrAndData=0;
  WriteConfig_xx.Page=0;
  WriteConfig_xx.Addr_Flash_OptionStart=Addr_Flash_OptionProtec;
  WriteConfig_xx.SecurityOption=1;
  WriteConfig_xx.SecurityBit=13;
  WriteConfig_xx.WriteCheckNeed=0;
  WriteConfig_xx.ReadCnt=1;
  WriteConfig_xx.ReadOkCnt=1;
  WriteConfig_xx.ReWriteCnt=3;
  WriteConfig_xx.ForceWriteCnt=1;
  WriteConfig_xx.AllCheckNeed=1;      

//  IRC adjust
//  if (DeviceConfig_xx.IRC_Select==0x01)
//  {
//    ReFlag=M201IRC(0x0118,DeviceConfig_xx.IRC_OPTION,0x1c05,DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
//    if(ReFlag == 0)
//    {
//      return 0;
//    }
//  }

//进烧写模式  
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(200); 
  if(OTP_Mode_IN(DeviceConfig_xx.MCU_ID,vpp12,vdd65)==0)	
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  }

  if (TestAdc()==0)
  {
    return 0;
  }

//Write ROM
  if (DeviceConfig_xx.WriteEn & 0x0f)
  {
    CheckSum=0;
    FM_CS_0;
    FMReadMore(Addr_Flash_ROMStart+DeviceConfig_xx.RomFirAddr-1);
    Fdata =Rxdata;
    
    id_length=iic_data[1];
    id_addr0=(iic_data[2]<<8)+iic_data[3];// ADDRL
    id_addr1=(iic_data[4]<<8)+iic_data[5];
    id_addr2=(iic_data[6]<<8)+iic_data[7];
    id_addr3=(iic_data[8]<<8)+iic_data[9];//ADDRH
    current_id=(iic_data[10]<<24)+(iic_data[11]<<16)+(iic_data[12]<<8)+iic_data[13];
    id_end=(iic_data[15]<<24)+(iic_data[16]<<16)+(iic_data[17]<<8)+iic_data[18];
    max_id=(iic_data[27]<<24)+(iic_data[28]<<16)+(iic_data[29]<<8)+iic_data[30]; //max-id

    if (current_id>max_id)
    {
      iic_data[10]=iic_data[23];
      iic_data[11]=iic_data[24];
      iic_data[12]=iic_data[25];
      iic_data[13]=iic_data[26];
    }
    if (iic_data[0]==0x0a)
    {
      if (OKcounter>=id_end)
      {
        ERORR_VALUE=OK_Counter_Full;
        return 0; // full limit
      }
    }

    for(OTP_ADDR=DeviceConfig_xx.RomFirAddr;OTP_ADDR<=DeviceConfig_xx.RomEndAddr;OTP_ADDR++)
    {
      SPI_ReadByte();
      Fdata=Rxdata;
      CheckSum +=Fdata;

      //2012.09.12 dynamic id
      if (iic_data[0]==0x0a) //id enable
      {
        if (OTP_ADDR==id_addr0)
        {
          Fdata=iic_data[13];
        }
        else if ((OTP_ADDR==id_addr1)&& (id_length>1))
        {
          Fdata=iic_data[12];
        }
        else if ((OTP_ADDR==id_addr2)&& (id_length>2))
        {
          Fdata=iic_data[11];
        }
        else if ((OTP_ADDR==id_addr3)&&(id_length>3))
        {
          Fdata=iic_data[10];
        }
      }
      Rxdata=Fdata;      
      
      ReCnt=WriteConfig_xx.ReWriteCnt;
      while(ReCnt--)  //start wr rom
      {	 
        ForceReCnt=WriteConfig_xx.ForceWriteCnt;
        if (Fdata!=0xff)
        {
          while(ForceReCnt--)
          {
            OTP_WriteByte(OTP_ADDR,Fdata);
            //OTP_ReadData=OTP_ReadByte(OTP_ADDR);
          }
        }
        SuccessCnt = 0;
        if(WriteConfig_xx.WriteCheckNeed)
        {
          for(i=0;i<WriteConfig_xx.ReadCnt;i++)
          {
            OTP_ReadData=OTP_ReadByte(OTP_ADDR);
            if (OTP_ReadData ==Fdata)
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
          return 0;
        }                   
      }     
    }
    FM_CS_1;
  }
//Write OPTION
  if (DeviceConfig_xx.WriteEn & 0x0f)
  {
    FM_CS_0;
    FMReadMore(Addr_Flash_OptionProtec-1);      
    Fdata =Rxdata;
    for(op_addr=0;op_addr<DeviceConfig_xx.OptionSize;op_addr++)//
    {
      SPI_ReadByte();
      Fdata=Rxdata;
      if(op_addr==0)
      {
        Fdata|=0x80;
      }
      else if(op_addr==1)
      {
        OPTION_FRT =OTP_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);
        if(OPTION_FRT==0xff)
        {       
          OPTION_FRT =OTP_ReadByte(DeviceConfig_xx.IRC_VALUE_ADDR);
          if ((OPTION_FRT == 0xff)||(OPTION_FRT==0x00))
          {
            OPTION_FRT =OTP_ReadByte(DeviceConfig_xx.IRC_VALUE_ADDR-2);
            if ((OPTION_FRT == 0xff)||(OPTION_FRT==0x00))
            {
              ERORR_VALUE=IRC_Value_0XFF;
              return 0;
            }
          }
        }
        Fdata=OPTION_FRT;
      }
      
      WriteConfig_xx.OptionValue[op_addr+WriteConfig_xx.Page] = Fdata;//烧option之前先把值存起来，以方便后续全面校验时读值
      WriteConfig_xx.OptionAddr[op_addr+WriteConfig_xx.Page]=DeviceConfig_xx.OptionAddr+op_addr+option_shift;
      ReCnt=WriteConfig_xx.ReWriteCnt;      
      while(ReCnt--)  //start wr rom
      {	 
        ForceReCnt=WriteConfig_xx.ForceWriteCnt;
        if (Fdata!=0xff)
        {
          while(ForceReCnt--)
          {
            OTP_WriteByte(DeviceConfig_xx.OptionAddr+op_addr,Fdata);
            //OTP_ReadData=OTP_ReadByte(OTP_ADDR);
          }
        }
        SuccessCnt = 0;
        if(WriteConfig_xx.WriteCheckNeed)
        {
          for(i=0;i<WriteConfig_xx.ReadCnt;i++)
          {
            OTP_ReadData=OTP_ReadByte(DeviceConfig_xx.OptionAddr+op_addr);
            if (OTP_ReadData ==Fdata)
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
          ERORR_VALUE=OPTION_Write_false;
          return 0;
        }                   
      }     
    }
    FM_CS_1;
  }

////check IRC 
//  OPTION_FRT=OTP_ReadByte(0x0001);
//  
//  POWER_OFF(vpp00,vdd00);
//  Delay_1ms(200);     
//  ReFlag=MC20P02BIRC_ModeInCheck(0x0118,DeviceConfig_xx.IRC_OPTION,0x1c05,DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);  
//  ConfigTIM4();
//  if(ReFlag == 0) 
//  {
//    ERORR_VALUE=IRC_Value_false;
//    return 0;
//  }   
//  
////进烧写模式 
//  POWER_OFF(vpp00,vdd00);
//  Delay_1ms(200);   
//  if(OTP_Mode_IN(DeviceConfig_xx.MCU_ID,vpp12,vdd65)==0)	
//  {
//    ERORR_VALUE=OTP_ModeIn_false;
//    return 0;
//  }
//
//  if (TestAdc()==0)
//  {
//    return 0;
//  }
  
//write checksum
  Fdata =CheckSum/0x100;
  OldIRC=CheckSum%0x100;
  ReCnt=0;
  while(++ReCnt<3)
  {		
    OTP_WriteByte(0x0010,Fdata);
    OTP_WriteByte(0x0011,OldIRC);
  }
  
//Check ROM
  if (DeviceConfig_xx.WriteEn & 0x0f)
  {
    CheckSum=0;
    FM_CS_0;
    FMReadMore(Addr_Flash_ROMStart+DeviceConfig_xx.RomFirAddr-1);
    Fdata =Rxdata;
    
    id_length=iic_data[1];
    id_addr0=(iic_data[2]<<8)+iic_data[3];// ADDRL
    id_addr1=(iic_data[4]<<8)+iic_data[5];
    id_addr2=(iic_data[6]<<8)+iic_data[7];
    id_addr3=(iic_data[8]<<8)+iic_data[9];//ADDRH
    current_id=(iic_data[10]<<24)+(iic_data[11]<<16)+(iic_data[12]<<8)+iic_data[13];
    id_end=(iic_data[15]<<24)+(iic_data[16]<<16)+(iic_data[17]<<8)+iic_data[18];
    max_id=(iic_data[27]<<24)+(iic_data[28]<<16)+(iic_data[29]<<8)+iic_data[30]; //max-id

    if (current_id>max_id)
    {
      iic_data[10]=iic_data[23];
      iic_data[11]=iic_data[24];
      iic_data[12]=iic_data[25];
      iic_data[13]=iic_data[26];
    }
    if (iic_data[0]==0x0a)
    {
      if (OKcounter>=id_end)
      {
        ERORR_VALUE=OK_Counter_Full;
        return 0; // full limit
      }
    }

    for(OTP_ADDR=DeviceConfig_xx.RomFirAddr;OTP_ADDR<=DeviceConfig_xx.RomEndAddr;OTP_ADDR++)
    {
      SPI_ReadByte();
      Fdata=Rxdata;
      //CheckSum +=Fdata;

      //2012.09.12 dynamic id
      if (iic_data[0]==0x0a) //id enable
      {
        if (OTP_ADDR==id_addr0)
        {
          Fdata=iic_data[13];
        }
        else if ((OTP_ADDR==id_addr1)&& (id_length>1))
        {
          Fdata=iic_data[12];
        }
        else if ((OTP_ADDR==id_addr2)&& (id_length>2))
        {
          Fdata=iic_data[11];
        }
        else if ((OTP_ADDR==id_addr3)&&(id_length>3))
        {
          Fdata=iic_data[10];
        }
      }
      if (WriteConfig_xx.AllCheckNeed == 1)
      {
        SuccessCnt = 0;
        for(j=0;j<WriteConfig_xx.ReadCnt;j++)
        {
          OTP_ReadData=OTP_ReadByte(OTP_ADDR);
          if (OTP_ReadData==Fdata)
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
          ERORR_VALUE=ROM_Write_false;
          return 0;
        }
      }
      
    }
    FM_CS_1;
  }
  
//check option 
  if(DeviceConfig_xx.WriteEn & 0xf0)
  {
    if(WriteConfig_xx.AllCheckNeed > 0)
    {
      for(op_addr=0;op_addr<DeviceConfig_xx.OptionSize;op_addr++)
      {
	Fdata=WriteConfig_xx.OptionValue[op_addr];
        Addres=WriteConfig_xx.OptionAddr[op_addr];

        if (WriteConfig_xx.AllCheckNeed == 1)
        {
          SuccessCnt = 0;
	  for(j=0;j<WriteConfig_xx.ReadCnt;j++)
	  {
            OTP_ReadData=OTP_ReadByte(Addres);
            if (OTP_ReadData==Fdata)
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
	    ERORR_VALUE=OPTION_Write_false;
	    OTP_ADDR= Addres;
            return 0;
	  }
        }
      }
    }
  }
  
  
//write protect bit
//write protect bit
  if(DeviceConfig_xx.WriteEn & 0xf0)
  {  
    FM_CS_0;
    FMReadMore(Addr_Flash_OptionProtec);    
    Fdata =Rxdata;

    ReCnt=WriteConfig_xx.ReWriteCnt;
    while(ReCnt--) 
    {	
      ForceReCnt=WriteConfig_xx.ForceWriteCnt;      
      if (Fdata != 0xff)  // 
      {
        while(ForceReCnt--)
        {
          OTP_WriteByte(DeviceConfig_xx.OptionAddr,Fdata);
        } 
      } 
      if(WriteConfig_xx.AllCheckNeed == 1)
      {
        SuccessCnt = 0;
        for(j=0;j<WriteConfig_xx.ReadCnt;j++)
        {
          OTP_ReadData=OTP_ReadByte(DeviceConfig_xx.OptionAddr);
          if (OTP_ReadData==Fdata)
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
          ERORR_VALUE=OPTION_Write_false;
          OTP_ADDR= DeviceConfig_xx.OptionAddr;
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

