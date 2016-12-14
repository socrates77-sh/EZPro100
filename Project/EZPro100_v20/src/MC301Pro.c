

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


u8 MC301_Program()
{
  u8 i,j;
  //u8  IRCData=0x80;
  u8  ReCnt=0;
  u16  Fdata,ROMReadData;
  u16 OldIRC_M301=0;
  u8	FdataL,FdataH;
  u8  ReFlag=0;
  u16  CheckSum=0;

  u8 id_length=0;
  u16 id_addr0,id_addr1,id_addr2,id_addr3,mcu_addr;
  u32 id_end,max_id,current_id;

  u8    ForceReCnt=0;  
  u8    SuccessCnt;   
  u8    op_addr=0;
  u8    option_shift=0;  
  u16   Addres;
  
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

//  //IRC adjust
//  if (DeviceConfig_xx.IRC_Select==0x01)
//  {
//    //ReFlag=M201IRC(0x0118,DeviceConfig_xx.IRC_OPTION,0x1c05,DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
//    ReFlag=M301IRC(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
//    if(ReFlag == 0)
//    {
//      return 0;
//    }
//  }

//进模式  
  POWER_OFF(vpp00,vdd00);
  OTP_SCK_0;
  OTP_SDI_0;  
  VDD65V_On;
  Delay_1ms(1);
  IO5V_On;
  Delay_1ms(10);
  CLN_VDD2GND_Off;
  VPP12V_On;

  Delay_1ms(5);
  // model in test 
  if (TestAdc()==0)
  {
    return 0;
  }

  Fdata=MC301_ReadByte(0X3FFF); //read MCU ID
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
    ROMReadData=MC301_ReadByte(0x0000); //set ADDR
    for(OTP_ADDR=DeviceConfig_xx.RomFirAddr;OTP_ADDR<=DeviceConfig_xx.RomEndAddr;)
    {
      SPI_ReadByte();
      FdataL=Rxdata;
      CheckSum +=FdataL;
      SPI_ReadByte();
      FdataH=Rxdata;
      CheckSum +=FdataH;

      FdataH=FdataH&0x3f;
      
      Fdata=FdataH*0x100+FdataL;

      //Fdata=Fdata & 0x3fff;
                        
      //2012.09.12 dynamic id
      mcu_addr=OTP_ADDR/2;
      OTP_ADDR=OTP_ADDR+2;
                         
      if (iic_data[0]==0x0a) //id enable
      {
        if (mcu_addr==id_addr0)
        {
          Fdata=FdataH*0x100+iic_data[13];
        }
        else if ((mcu_addr==id_addr1)&& (id_length>1))
        {
          Fdata=FdataH*0x100+iic_data[12];
        }
        else if ((mcu_addr==id_addr2)&& (id_length>2))
        {
          Fdata=FdataH*0x100+iic_data[11];
        }
        else if ((mcu_addr==id_addr3)&&(id_length>3))
        {
          Fdata=FdataH*0x100+iic_data[10];
        }
      }
      Rxdata=Fdata;
      
      ReCnt=WriteConfig_xx.ReWriteCnt;
      while(ReCnt--)  //start wr rom
      {	 
        ForceReCnt=WriteConfig_xx.ForceWriteCnt;
        if (Fdata!=WriteConfig_xx.RomBit)
        {
          while(ForceReCnt--)
          {
            MC301_W(Fdata);
          }
        }
        SuccessCnt = 0;
        if(WriteConfig_xx.WriteCheckNeed)
        {
          for(i=0;i<WriteConfig_xx.ReadCnt;i++)
          {
            ROMReadData=MC301_R();
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
      }      
    }
    FM_CS_1;
  }
//Write OPTION  
  if(DeviceConfig_xx.WriteEn & 0xf0)
  {
    FM_CS_0;
    //FMReadMore(WriteConfig_xx.Addr_Flash_OptionStart+3);
    FMReadMore(WriteConfig_xx.Addr_Flash_OptionStart+WriteConfig_xx.Page*2-1);    
    Fdata =Rxdata;
    
    ROMReadData=MC301_ReadByte(DeviceConfig_xx.OptionAddr); //set ADDR
    for(op_addr=0;op_addr<DeviceConfig_xx.OptionSize;op_addr++)//
    {
      SPI_ReadByte();//从flash中取一个字节
      FdataL=Rxdata;
      SPI_ReadByte();//再从flash中取一个字节
      FdataH=Rxdata;

      //option1 
      if(op_addr==1)
      {
        FdataH|=0x20;
        OPTION_FRT=MC301_ReadByte(DeviceConfig_xx.IRC_VALUE_ADDR);
        //OPTION_FRT=OPTION_FRT & 0x3ff;
        if ((OPTION_FRT == 0xff)||(OPTION_FRT==0x00))
        {
          OPTION_FRT =MC301_ReadByte(DeviceConfig_xx.IRC_VALUE_ADDR-2);
          //OPTION_FRT=OPTION_FRT & 0x3ff;
          if ((OPTION_FRT == 0xff)||(OPTION_FRT==0x00))
          {
            ERORR_VALUE=IRC_Value_0XFF;
            return 0;
          }
        }
        OldIRC_M301=MC301_ReadByte(0X2001);
        OldIRC_M301=OldIRC_M301 & 0x00ff;
        if (OldIRC_M301==0x00ff)
        {
          FdataL =OPTION_FRT; //IRC from QC test
        }
        else
        {
          FdataL =OldIRC_M301;
        }        
      }
      
      Fdata=FdataH*0x100+FdataL;//组成一个字
      Fdata&=WriteConfig_xx.RomBit;    
      
      WriteConfig_xx.OptionValue[op_addr+WriteConfig_xx.Page] = Fdata;//烧option之前先把值存起来，以方便后续全面校验时读值
      WriteConfig_xx.OptionAddr[op_addr+WriteConfig_xx.Page]=DeviceConfig_xx.OptionAddr+op_addr+option_shift;
      ReCnt=WriteConfig_xx.ReWriteCnt;
      while(ReCnt--)  
      {	
        ForceReCnt=WriteConfig_xx.ForceWriteCnt;
        if (Fdata != WriteConfig_xx.RomBit)  // 
        {
          while(ForceReCnt--)
          {
            MC301_WriteByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift,Fdata);
          }
        }        
	SuccessCnt = 0;
	if (WriteConfig_xx.WriteCheckNeed)
	{
	  for(i=0;i<WriteConfig_xx.ReadCnt;i++)
	  {
	    ROMReadData=MC301_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);         
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

////check IRC
//  OPTION_FRT=MC301_ReadByte(0X2001);
//  ReFlag=M301IRC_ModeInCheck(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
//  ConfigTIM4();
//  if(ReFlag == 0) 
//  {
//    ERORR_VALUE=IRC_Value_false;
//    return 0;
//  }  
//  
//  POWER_OFF(vpp00,vdd00);
//  OTP_SCK_0;
//  OTP_SDI_0;  
//  VDD65V_On;
//  Delay_1ms(1);
//  IO5V_On;
//  Delay_1ms(10);
//  CLN_VDD2GND_Off;
//  VPP12V_On;
//
//  Delay_1ms(5);
//  // model in test 
//  if (TestAdc()==0)
//  {
//    return 0;
//  }
//
//  Fdata=MC301_ReadByte(0X3FFF); //read MCU ID
//  if (Fdata !=DeviceConfig_xx.MCU_ID)
//  {
//    ERORR_VALUE=OTP_ModeIn_false;
//    return 0;
//  }
  
//write checksum
  ReCnt=0;
  while(++ReCnt<3)
  {
    FdataH=CheckSum>>8;
    FdataL=CheckSum;

    MC301_WriteByte(DeviceConfig_xx.OptionAddr+0x0E,FdataH);
    MC301_WriteByte(DeviceConfig_xx.OptionAddr+0x0F,FdataL);
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
    ROMReadData=MC301_ReadByte(0x0000); //set ADDR
    for(OTP_ADDR=DeviceConfig_xx.RomFirAddr;OTP_ADDR<=DeviceConfig_xx.RomEndAddr;)
    {
      SPI_ReadByte();
      FdataL=Rxdata;
      CheckSum +=FdataL;
      SPI_ReadByte();
      FdataH=Rxdata;
      CheckSum +=FdataH;

      FdataH=FdataH&0x3f;
      
      Fdata=FdataH*0x100+FdataL;

      //Fdata=Fdata & 0x3fff;
                        
      //2012.09.12 dynamic id
      mcu_addr=OTP_ADDR/2;
      OTP_ADDR=OTP_ADDR+2;
                         
      if (iic_data[0]==0x0a) //id enable
      {
        if (mcu_addr==id_addr0)
        {
          Fdata=FdataH*0x100+iic_data[13];
        }
        else if ((mcu_addr==id_addr1)&& (id_length>1))
        {
          Fdata=FdataH*0x100+iic_data[12];
        }
        else if ((mcu_addr==id_addr2)&& (id_length>2))
        {
          Fdata=FdataH*0x100+iic_data[11];
        }
        else if ((mcu_addr==id_addr3)&&(id_length>3))
        {
          Fdata=FdataH*0x100+iic_data[10];
        }
      }

      Rxdata=Fdata;
      ReCnt=0;
      while(++ReCnt<6) 
      {		
        ROMReadData=MC301_R();
        //ROMReadData=Fdata;
        if (ROMReadData==Fdata)
        {
          ReCnt=0;
          break;
        }
        else if (ReCnt==4)
        {
          ERORR_VALUE=IRC_Write_false;
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

//Check option 
  if(DeviceConfig_xx.WriteEn & 0xf0)
  {
    if(WriteConfig_xx.AllCheckNeed > 0)
    {
      for(op_addr=0;op_addr<DeviceConfig_xx.OptionSize+WriteConfig_xx.Page;op_addr++)
      {
	Fdata=WriteConfig_xx.OptionValue[op_addr];
        Addres=WriteConfig_xx.OptionAddr[op_addr];
        SuccessCnt = 0;
        for(j=0;j<WriteConfig_xx.ReadCnt;j++)
        {
          ROMReadData=MC301_ReadByte(Addres);
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
          ERORR_VALUE=OPTION_NOMALL_ERROR;
          OTP_ADDR= Addres;
          return 0;
        }
      }
    }
  }
  
//write protect bit
  if(DeviceConfig_xx.WriteEn & 0xf0)
  {  
    FM_CS_0;
    FMReadMore(WriteConfig_xx.Addr_Flash_OptionStart+WriteConfig_xx.Page*2+WriteConfig_xx.SecurityOption*2-1);    

    Fdata =Rxdata;
    SPI_ReadByte();//从flash中取一个字节
    FdataL=Rxdata;
    FdataL=MC301_ReadByte(0X2001);
    SPI_ReadByte();//再从flash中取一个字节
    FdataH=Rxdata;
    Fdata=FdataH*0x100+FdataL;//组成一个字 

    ReCnt=WriteConfig_xx.ReWriteCnt;
    while(ReCnt--) 
    {	
      ForceReCnt=WriteConfig_xx.ForceWriteCnt;      
      if (Fdata != WriteConfig_xx.RomBit)  // 
      {
        while(ForceReCnt--)
        {
          MC301_WriteByte(DeviceConfig_xx.OptionAddr+WriteConfig_xx.SecurityOption+option_shift,Fdata);
        } 
      } 
      if(WriteConfig_xx.AllCheckNeed == 1)
      {
        SuccessCnt = 0;
        for(j=0;j<WriteConfig_xx.ReadCnt;j++)
        {
          ROMReadData=MC301_ReadByte(DeviceConfig_xx.OptionAddr+WriteConfig_xx.SecurityOption+option_shift);
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

void MC301_Instruct(u16 OTPData)
{
  u8 i,temp,a;

  OTP_SCK_0;
  for(i=0;i<6;i++)
  {
    OTP_SCK_1;
    a = OTPData >>i;
    if ((a & 0x01)==1)
    {
            OTP_SDI_1;
    } 
    else
    {
            OTP_SDI_0;
    }
    NOP;
    OTP_SCK_0;
    NOP;
  }
}

u16 MC301_R( )
{
        u8 i,temp; //temp use for Nop instruce
		u16 a,read_data;
	//read 
	MC301_Instruct(DataRe);
	NOP;
	NOP;
	OTP_SCK_1;
	NOP;
	OTP_SCK_0;
	NOP;
	read_data=0;
	for(i=0;i<14;i++)
	{
		OTP_SCK_1;  //1
		NOP;
		a=GPIO_ReadInputDataBit(OTP_SDO_PORT,OTP_SDO_PIN);
		read_data |= a<<i;

		OTP_SCK_0;//D7
		NOP;
	}
	OTP_SCK_1;
	NOP;
	OTP_SCK_0;
	NOP;

	return read_data;

}
void MC301_W(u16 OTPData)
{
	  u8 i,temp;
      u16 wData; 
	MC301_Instruct(DataWr);

	OTP_SCK_1;
	NOP;
	OTP_SCK_0;
	NOP;
	for(i=0;i<14;i++)
	{
		OTP_SCK_1;  //1

		wData =OTPData>>i;
		if ((wData & 0x0001)==1)
		{
			OTP_SDI_1;
		} 
		else
		{
			OTP_SDI_0;
		}
		NOP;
		OTP_SCK_0;
		NOP;
	}
	OTP_SCK_1;
	NOP;
	OTP_SCK_0;
	NOP;

	MC301_Instruct(ProStr);
	Delay_100us(DeviceConfig_xx.ProTime);
	MC301_Instruct(ProEnd);

}

void MC301_WriteByte(u16 OTPAddr,u16 OTPData)
{
	u8 i,temp;
	u16 a;
	MC301_Instruct(SetAddr);

	OTP_SCK_1;
	NOP;
	OTP_SCK_0;
	NOP;
	for(i=0;i<14;i++)
	{
		OTP_SCK_1;  //1

		a =OTPAddr>>i;
		if ((a & 0x0001)==1)
		{
			OTP_SDI_1;
		} 
		else
		{
			OTP_SDI_0;
		}
		NOP;
		OTP_SCK_0;
		NOP;
	}
	OTP_SCK_1;
	NOP;
	OTP_SCK_0;
	NOP;
	
	MC301_W(OTPData);
	//MC301_Instruct(ProStr);
	//Delay_100us(1);
	//MC301_Instruct(ProEnd);
}

u16 MC301_ReadByte(u16 OTPAddr)
{
	u8 i,temp;
	u16 a;

	MC301_Instruct(SetAddr);
	NOP;
	NOP;
	OTP_SCK_1;
	NOP;
	OTP_SCK_0;
	NOP;
	for(i=0;i<14;i++)
	{
		OTP_SCK_1;  //1

		a =OTPAddr>>i;
		if ((a & 0x0001)==1)
		{
			OTP_SDI_1;
		} 
		else
		{
			OTP_SDI_0;
		}
		NOP;
		OTP_SCK_0;
		NOP;
	}
	OTP_SCK_1;
	NOP;
	OTP_SCK_0;
	NOP;
	
	a=MC301_R();

	return a;
}


//mc30p01 irc
////IRC mainfuntion
//u8 M301IRC(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
//{
//
//	u8 i;
//	u8 mm=0x80;
//       // u16 id;
//
//	POWER_OFF(vpp00,vdd00);
////vdd 5.0v
//	//MCP42050_ADJ(ADJ_VDD,51);
//	OTP_SCK_0;
//	OTP_SDI_0;
//
//	VDD30V_On;
//	Delay_1ms(1); 
//	IO5V_Off;
//	Delay_1ms(10);
//	CLN_VDD2GND_Off;
//	VPP12V_On;
//
//	Delay_1ms(5);
//	
//	//id=MC301_ReadByte(0X3FFF); //read MCU ID
//
//        MC301_Instruct(TestMod);
//	for(i=0;i<5;i++)
//	{
//		MC301_IRC_INST(0X0000); //5 NOP
//	}
//	
//	MC301_IRC_INST(0x0b10); //	movai 0x00 ; Firc/128
//	MC301_IRC_INST(0x15f2); //	movra 0x72
//	
//	MC301_IRC_INST(0x0b60); //	movra 0x60 ; p62 output Firc/128
//	MC301_IRC_INST(0x15f0); //	movra 0x70 ;
//	
//
//
//	OPTION_FRT=0; //initial 
//	for(i=0;i<8;i++)
//	{
//		//mm=0xff;
//		//M201FTSendData(IRC_Option,IRC_Addr,mm);
//		MC301FTtest(mm);
//		if (IRC_VALUE == IRC_FreqType)
//		{
//			break;
//		}
//		if (IRC_VALUE<IRC_FreqType)
//		{
//			mm |=0x80 >>i;
//		} 
//		else
//		{
//			mm &=~(0x80>>i);
//		}
//		mm |=0x40>>i;
//	}
//
//	MC301FTtest(mm);
//
//	if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
//	{
//		OPTION_FRT=mm;
//		return 1;
//	} 
//	else
//	{
//		OPTION_FRT=0XFF;
//		return 0;
//	}
//}
//

u8 M301IRC_ModeInCheck(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{

  u8 i;
  u8 mm=0x80;

  MC301_Instruct(TestMod);
  for(i=0;i<5;i++)
  {
    MC301_IRC_INST(0X0000); //5 NOP
  }

  MC301_IRC_INST(0x0b10); //	movai 0x00 ; Firc/128
  MC301_IRC_INST(0x15f2); //	movra 0x72

  MC301_IRC_INST(0x0b60); //	movra 0x60 ; p62 output Firc/128
  MC301_IRC_INST(0x15f0); //	movra 0x70 ;
	
  mm=OPTION_FRT;
  MC301FTtest(mm);

  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    OPTION_FRT=mm;
    return 1;
  } 
  else
  {
    OPTION_FRT=0XFF;
    return 0;
  }
}


void MC301FTtest(u8 IRC_Opiton)
{
	u16 temp=0x0b00;
	u16 i;
	u16 cnt[3];

	temp=temp+IRC_Opiton;
	MC301_IRC_INST(temp); //	movra 0x60 ; p62 output Firc/128
	MC301_IRC_INST(0x15f1); //	movra 0x70 ;

	MC301_IRC_INST(0X0000); //NOP

	//-------------------------
	Delay_1ms(10); //wait same time

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

	//-----------------------------
}

void MC301_IRC_INST(u16 OTPData)
{
	u8 i,temp;
	u16 wData; 
	//MC301_Instruct(DataWr);

	OTP_SCK_1;
	NOP;
	OTP_SCK_0;
	NOP;
	for(i=0;i<14;i++)
	{
		OTP_SCK_1;  //1

		wData =OTPData>>i;
		if ((wData & 0x0001)==1)
		{
			OTP_SDI_1;
		} 
		else
		{
			OTP_SDI_0;
		}
		NOP;
		OTP_SCK_0;
		NOP;
	}
	OTP_SCK_1;
	NOP;
	OTP_SCK_0;
	NOP;

	//4 CLK Q1,Q2,Q3,Q4
	OTP_SCK_1;
	NOP;
	OTP_SCK_0; //Q1
	NOP;
	OTP_SCK_1;
	NOP;
	OTP_SCK_0;//Q2
	NOP;
	OTP_SCK_1;
	NOP;
	OTP_SCK_0;//Q3
	NOP;
	OTP_SCK_1;
	NOP;
	OTP_SCK_0;//Q4
	NOP;

}

//u16 MC301_ReadS()
//{
//	u16 a;
//	MC301_Instruct(AddrINC);
//	//read 
//	a=MC301_R();
//	return a;
//
//}
//
//void MC301_WriteS(u16 OTPData)
//{
//
//	MC301_Instruct(AddrINC);
//	MC301_W(OTPData);
//
//}