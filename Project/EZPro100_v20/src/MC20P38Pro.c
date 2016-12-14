
#include "stm32f10x.h"
#include "global.h"
#include "spi.h"
#include "erorrNum.h"
//#include "OTPRW.h"
#include "power.h"
#include "IRC.h"
#include "MC20P38IRC.h"
//#include "MC224BPro.h"
#include "Config.h"
#include "ADC.h"
#include "OTPRW_8K.h"
#include "wx_i2c.h"

u8 MC20P38_Program()
{
//	u16 i;
	u8  OldIRC=0;
	u8  ReCnt=0;
	u8  Fdata;
        u8  ReFlag=0;
        u16 CheckSum=0;

		u8 id_length=0;
		u16 id_addr0,id_addr1,id_addr2,id_addr3;
		u32 id_end,current_id,max_id;

	//IRC adjust

	if (DeviceConfig_xx.IRC_Select==0x01)
	{
		ReFlag=M238IRC(0x0118,DeviceConfig_xx.IRC_OPTION,0x2c01,DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
                if(ReFlag == 0)
                {
                  return 0;
                }
		//if ((OPTION_FRT == 0xff) || (OPTION_FRT == 0x00) )
		//{
		//	return 0;
		//}
	}

	//power on
	if(OTP8K_Mode_IN(DeviceConfig_xx.MCU_ID,vpp12,vdd65)==0)	
	{
		ERORR_VALUE=OTP_ModeIn_false;
		return 0;
	}

	if (TestAdc()==0)
	{
		return 0;
	}
	//-----------------------------------------

	//Write IRC option
	OP_STATE=OP_L; //select option rom @2012.12.14

	if (DeviceConfig_xx.OptionSize >= 2)
	{
		if (DeviceConfig_xx.IRC_Select==0x02)
		{
			OPTION_FRT =OTP8K_ReadByte(DeviceConfig_xx.IRC_VALUE_ADDR);
			if ((OPTION_FRT == 0xff)||(OPTION_FRT==0x00))
			{
				OPTION_FRT =OTP8K_ReadByte(DeviceConfig_xx.IRC_VALUE_ADDR-2);
				if ((OPTION_FRT == 0xff)||(OPTION_FRT==0x00))
				{
					ERORR_VALUE=IRC_Value_0XFF;
					return 0;
				}
			}
		 }

         OldIRC=OTP8K_ReadByte(0x0001);
		 if (OldIRC == 0xff)
		{
			//write IRC
			while(++ReCnt<6)
			{		
				OTP8K_WriteByte(0x0001,OPTION_FRT);
				if (OTP8K_ReadByte(0x0001) == OPTION_FRT)
				{
					ReCnt=0;
					break;
				}
				else if (ReCnt==4)
				{
					ERORR_VALUE=IRC_Write_false;
					return 0;
				}
			 }
		/*	if (OldIRC>(OPTION_FRT+6) || OldIRC<(OPTION_FRT-6))
			{
				return 0;
			}*/
			
		 }
		//else if((OPTION_FRT>=(OldIRC-1))&&(OPTION_FRT<=(OldIRC+1)))
		//{
		//	ReCnt=0;
		//}
	}

	//----------------------------------------------------
	//--------2012.05.16 ----------------------------
	if (DeviceConfig_xx.IRC_Select==0x03)
	{
		OPTION_FRT =OTP8K_ReadByte(0x0001);
		if ((OPTION_FRT == 0xff)||(OPTION_FRT==0x00))
		{
			ERORR_VALUE=IRC_Value_0XFF;
			return 0;
		}
	}

	//Write ROM
  OP_STATE=OP_H; //select rom addr @2012.12.14
  if(DeviceConfig_xx.WriteEn & 0x0f)
 { //temp add code
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


		if (DeviceConfig_xx.MCU_Type==MCU_Type_OTP)
		{
			if(Fdata ==0xff)
			{
				OTP_ReadData=OTP8K_ReadByte(OTP_ADDR);
				if ( OTP_ReadData != 0xff)
				{
					ERORR_VALUE=ROM_Write_false;
					return 0;
				}
				continue;
			}
		}
//		i=0;
		while(++ReCnt<8)
		{	
			OTP8K_WriteByte(OTP_ADDR,Fdata);
			OTP_ReadData=OTP8K_ReadByte(OTP_ADDR);

			if (OTP_ReadData ==Fdata)
			{
				ReCnt=0;
				break;
			}
			else if (ReCnt==6) 
			{
				ERORR_VALUE=ROM_Write_false;
				return 0;
			}
		}
	}
	FM_CS_1;
        //Write checkSum
        Fdata =CheckSum/0x100;
        OldIRC=CheckSum%0x100;

		OP_STATE=OP_L; //write data to option add
       	while(++ReCnt<6)
	{		
		OTP8K_WriteByte(0x0010,Fdata);
                OTP8K_WriteByte(0x0011,OldIRC);
		OTP_ReadData=OTP8K_ReadByte(0x0010);
                ReFlag =OTP8K_ReadByte(0x0011);
		if ((OTP_ReadData ==Fdata ) && (ReFlag==OldIRC))
		{
			ReCnt=0;
			break;
		}
		else if (ReCnt==4)
		{
			ReCnt=0;
			break;
		}
	}
        
   }//Write ROM 
   
//Write Option
   OP_STATE=OP_L;
   if(DeviceConfig_xx.WriteEn & 0xf0)
  {
	  //protect option
	  if (FMReadOne(Addr_Flash_OptionProtec8k)==1)
	  {
		Fdata =Rxdata;
	  }
	  else
	  {
		  ERORR_VALUE=FM_Read_false;
		return 0;
	  }
	  while(++ReCnt<6)
	  {		
            	//	OTP8K_WriteByte(0x0000,0Xf3);
		//if (OTP8K_ReadByte(0x0000) != 0Xf3)
		//{
		//	return 0;
		//}
		OTP8K_WriteByte(DeviceConfig_xx.OptionAddr,Fdata);
                OTP_ReadData=OTP8K_ReadByte(DeviceConfig_xx.OptionAddr); 
		if (OTP_ReadData==Fdata)
		{
			ReCnt=0;
			break;
		}
		else if (ReCnt==4)
		{
			//POWER_OFF(vpp00,vdd00);
			OTP_ADDR=DeviceConfig_xx.OptionAddr;
			ERORR_VALUE=ROM_Write_false;
			return 0;
		}
	  }
          
    }
	POWER_OFF(vpp00,vdd00);
	if (ReCnt>=5)
	{
		ERORR_VALUE=Program_Erorr;
		return 0;
	}
	Delay_1ms(200);
        return 1;     
}
