

#include "stm32f10x.h"
#include "global.h"
#include "spi.h"
#include "OTPRW.h"
#include "power.h"
#include "IRC.h"
#include "MC20P38Pro.h"
#include "Config.h"
#include "ADC.h"
#include "OTPRW_8K.h"
#include "LCD.h"

//const u16 ProgTime=1500; 
/*
void OTPInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;


	//initial PC4 -> OTP_SCK,PC5 ->SDI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7 | GPIO_Pin_8 |GPIO_Pin_9 |GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	//initial PE4 -> OTP_SDO
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_Init(GPIOE,&GPIO_InitStructure);

	//Initial CP, TIME_REF,FT
	OTP_SCK_0;
	OTP_SDI_0;
	OTP_DISA_0;
	OTP_DISB_0;
	OTP_DISC_0;
	OTP_DISD_0;


}*/
u8 OTP8K_Mode_s(u16 OTPAddr)
{
	u8 i=12;
	u8 a=0;
	u8 redata=0;
	u16 temp=0;

	OTP_SDI_1;

	OTP_SCK_0;	//RW
	NOP;
	OTP_SCK_1;
	NOP;
	for(i=14;i>0;i--)
	{
		OTP_SCK_0;  //A13

		temp =OTPAddr>>(i-1);
		if ((temp & 0x0001)==1)
		{
			OTP_SDI_1;
		} 
		else
		{
			OTP_SDI_0;
		}
		NOP;
		OTP_SCK_1;
		NOP;
	}
	//NOP;
	OTP_SDI_0;

	OTP_SCK_0 ; //ACK1
	NOP;
	OTP_SCK_1;
	NOP;
	OTP_SCK_0;	//ACK2
	NOP;
	OTP_SCK_1;
	NOP;
	OTP_SCK_0;	//SP
	NOP;
	OTP_SCK_1;
	NOP;
	for(i=8;i>0;i--)
	{
		OTP_SCK_0;		
		a=GPIO_ReadInputDataBit(OTP_SDO_PORT,OTP_SDO_PIN);
		redata |= a<<(i-1);
		NOP;
		OTP_SCK_1;//D7
		NOP;
	}
	//OTP_SCK_0; //D0
	//OTP_SCK_1;
	//NOP;
	OTP_SCK_0 ; //WT1
	NOP;
	OTP_SCK_1;
	NOP;
	OTP_SCK_0;	//PRG
	NOP;
	OTP_SCK_1;
	NOP;
	OTP_SCK_0;	//WT2
	NOP;
	OTP_SCK_1;
	NOP;
	return redata;
}


u8 OTP8K_ReadByte(u16 OTPAddr)
{
	u8 i=12;
	u8 a=0;
	u8 redata=0;
	u16 temp=0;

	OTP_SDI_1;

	OTP_SCK_0;	//RW
	NOP;
	OTP_SCK_1;
	NOP;
	for(i=14;i>0;i--)
	{
		OTP_SCK_0;  //A13

		temp =OTPAddr>>(i-1);
		if ((temp & 0x0001)==1)
		{
			OTP_SDI_1;
		} 
		else
		{
			OTP_SDI_0;
		}
		NOP;
		OTP_SCK_1;
		NOP;
	}
	//NOP;
	
	if (OP_STATE==1)
	{
		OTP_SDI_1; //select otp,but not option
	}
	else
	{
		OTP_SDI_0;
	}
	NOP;
	OTP_SCK_0 ; //ACK1	
	NOP;
	OTP_SCK_1;
	temp =GPIO_ReadInputDataBit(OTP_SDO_PORT,OTP_SDO_PIN);
	if(temp==1)
	{
		return 0x00;
	}
	NOP;
	OTP_SCK_0;	//ACK2
	NOP;
	OTP_SCK_1;
	temp =GPIO_ReadInputDataBit(OTP_SDO_PORT,OTP_SDO_PIN);
	if(temp==0)
	{
		return 0x00;
	}
	NOP;
	OTP_SCK_0;	//SP
	NOP;
	OTP_SCK_1;
	NOP;
	for(i=8;i>0;i--)
	{
		OTP_SCK_0;		
		a=GPIO_ReadInputDataBit(OTP_SDO_PORT,OTP_SDO_PIN);
		redata |= a<<(i-1);
		NOP;
		OTP_SCK_1;//D7
		NOP;
	}
	//OTP_SCK_0; //D0
	//OTP_SCK_1;
	//NOP;
	OTP_SCK_0 ; //WT1
	NOP;
	OTP_SCK_1;
	NOP;
	OTP_SCK_0;	//PRG
	NOP;
	OTP_SCK_1;
	NOP;
	OTP_SCK_0;	//WT2
	NOP;
	OTP_SCK_1;
	NOP;
	return redata;
}


//--------------------------------------
//Ð´Ò»¸ö×Ö½Ú
//
//--------------------------------------
void OTP8K_WriteByte(u16 OTPAddr,u8 OTPData)
{
	u8 i;
	u16 cnt=0;
	u16 temp=0;
	OTP_SDI_1;
	NOP;
	OTP_SDI_0; 
	OTP_SCK_0; //RW
	NOP;
	OTP_SCK_1;
	NOP;
	for(i=14;i>0;i--)
	{
		OTP_SCK_0;  //A12

		temp =OTPAddr>>(i-1);
		if ((temp & 0x0001)==1)
		{
			OTP_SDI_1;
		} 
		else
		{
			OTP_SDI_0;
		}
		NOP;
		OTP_SCK_1;
		NOP;
	}
	if (OP_STATE==0)
	{
		OTP_SDI_0;
	}
	else
	{
		OTP_SDI_1;
	}
	NOP;
	OTP_SCK_0 ; //ACK1
	NOP;
	OTP_SCK_1;
	NOP;
	OTP_SCK_0;	//ACK2
	NOP;
	OTP_SCK_1;
	NOP;
	OTP_SCK_0;	//SP
	NOP;
	OTP_SCK_1;
	NOP;
	for(i=8;i>0;i--)
	{
		OTP_SCK_0;  //D0
		temp = OTPData >>(i-1);
		if ((temp & 0x01)==1)
		{
			OTP_SDI_1;
		} 
		else
		{
			OTP_SDI_0;
		}
		NOP;
		OTP_SCK_1;//D7
		NOP;
	}
	//OTP_SCK_0; //D0
	//OTP_SCK_1;
	//OTP_SDI_1;
	OTP_SCK_0 ; //WT1
	NOP;
	OTP_SCK_1;
	NOP;
	if (DeviceConfig_xx.MCU_Type==MCU_Type_OTP)
	{
		OTP_SCK_0;	//PRG
		cnt=0;
		while(cnt<DeviceConfig_xx.ProTime) cnt++;
		OTP_SCK_1;
		cnt=0;
		while(cnt<DeviceConfig_xx.ProTime) cnt++;
		OTP_SCK_0;	//WT2
		NOP;
		OTP_SCK_1;
		NOP;
	} 
	else if (DeviceConfig_xx.MCU_Type==MCU_Type_MTP)
	{
		OTP_SCK_0;	//WT2
		NOP;
		OTP_SCK_1;
		NOP;
		OTP_SCK_0;	//PRG
                cnt=0;
		while(cnt<60000) cnt++;

		//while(GPIO_ReadInputDataBit(OTP_BUSY_PORT,OTP_BUSY_PIN)==1);

		OTP_SCK_1;
                cnt=0;
		while(cnt<60000) cnt++;
	}
}


u8 OTP8K_Mode_IN(u16 OTPID,u8 vpp,u8 vdd)
{
	//u16 i;
	u8 a;
	if (DeviceConfig_xx.MCU_Type==MCU_Type_OTP)
	{
		POWER_ON(vpp,vdd);
	}
	else
	{
		POWER_ON(vpp,vdd30);
	}
	//Delay_1ms(3);//20
	displaWrite_Check();
	
	OTP8K_Mode_s(OTPID);
	
	OP_STATE=0;
	a=OTP8K_ReadByte(0x0003); // Write OTP 
	//test if come in the WriteMode sucess?
	if (a==0x00)
	{
		//ModelIn_result=0;
		return 0;
	}
	return 1;

}


//Verify
u8 OTP8K_Verify( )
{
	u8 VerifyData;
	u8 ReadData;
	//u32 FMAddr=0x010000;
	//u32 FMOptionAddr =0x012000;
	u16 i;
	u8 Wcounter=0;


	//power on
	if(OTP8K_Mode_IN(DeviceConfig_xx.MCU_ID,vpp12,vdd65)==0)	
	{
		return 0;
	}
	if (TestAdc()==0)
	{
		return 0;
	}


	for (Wcounter=0;Wcounter<3;Wcounter++)
	{

		// Option value verify
		if(DeviceConfig_xx.WriteEn & 0x0f)
		{
			ReadData=OTP8K_ReadByte(DeviceConfig_xx.OptionAddr);

			if (FMReadOne(Addr_Flash_Option8k)==1)
			{
				VerifyData=Rxdata;
			}
			else
			{ 
				return 0;
			}
			if (ReadData != VerifyData)
			{
				return 0;
			} 
		}
		if(DeviceConfig_xx.WriteEn & 0xf0)
		{
			// Select flash
			FM_CS_0;
			FMReadMore(Addr_Flash_ROMStart+DeviceConfig_xx.RomFirAddr-1);
			VerifyData=Rxdata;
			//ROM value verify	
			for(i=DeviceConfig_xx.RomFirAddr;i<=DeviceConfig_xx.RomEndAddr;i++)
			{
				ReadData = OTP8K_ReadByte(i);

				//read flash data
				SPI_ReadByte();
				VerifyData=Rxdata;

				// compare rom & flash data
				if (ReadData != VerifyData)
				{
					FM_CS_1;
					OTP_ADDR=i;
					return 0;
				}

			}
			FM_CS_1;
		}

	} //3 times verify
	if(DeviceConfig_xx.WriteEn & 0xf0)
	{
		if (FMReadOne(Addr_Flash_OptionProtec8k)==1)
		{
			VerifyData =Rxdata;
		}
		else
		{
			return 0;
		}
		Wcounter=0;
		while(++Wcounter<5)
		{		
			OTP8K_WriteByte(DeviceConfig_xx.OptionAddr,VerifyData);
			if (OTP8K_ReadByte(DeviceConfig_xx.OptionAddr) ==VerifyData)
		 {
			 Wcounter=0;
			 break;
			}
			else if (Wcounter==4)
			{
				//POWER_OFF(vpp00,vdd00);
				OTP_ADDR=DeviceConfig_xx.OptionAddr;
				return 0;
			}
		}
	}
	POWER_OFF(vpp00,vdd00);
	//Delay_1ms(200);
	return 1;

}

//OTP BLANK
u8 OTP8K_Blank(u16 ROMFirstAddr,u16 ROMEndAddr,u16 OPTIONFirstAddr)
{

	//u32 FMAddr=0x010000;
	//u32 FMOptionAddr =0x012000;
	//u16 i;

	//MODEL IN 
	OTP8K_Mode_IN(DeviceConfig_xx.MCU_ID,vpp12,vdd65);

	OTP_ADDR=OPTIONFirstAddr;
	// Option value verify
	OTP_ReadData=OTP8K_ReadByte(OTP_ADDR);

	if (OTP_ReadData !=DeviceConfig_xx.BlankCode)
	{
		return 0;
	}
	//ROM value verify	
	for(OTP_ADDR=ROMFirstAddr;OTP_ADDR<=ROMEndAddr;OTP_ADDR++)
	{
		OTP_ReadData = OTP8K_ReadByte(OTP_ADDR);

		// compare rom & Blank data
		if (OTP_ReadData != DeviceConfig_xx.BlankCode)
		{
			return 0;
		}

	}

	//power off
	POWER_OFF(vpp00,vdd00);
	Delay_1ms(50);
	return 1;

}

//------------------------------------------
u8 OTP8K_DownLoad(u16 MCU_ID,u16 MCU_FirAdd)
{
	u8 ResultFlag;
	u8 ReadData;
	u32 i;

	if (FMReadID() !=1)
	{
		return 0;
	}
	FMWriteStatus(BP_MemoryArray0); //clear protect of all
	FMChipErase();

	//otp 
	if (OTP8K_Mode_IN(MCU_ID,vpp12,vdd65)==0)
	{
		return 0;
	}

	//Read Option value
	ReadData = OTP8K_ReadByte(DeviceConfig_xx.OptionAddr);

	//Write it to Flash 
	FMWriteOne(Addr_Flash_Option,ReadData);

	//Read ROM value
	//for(i=Addr_Flash_ROMStart;i<Addr_OTP_ROMEnd;i++)
	for(i=MCU_FirAdd;i<=DeviceConfig_xx.RomEndAddr;i++)
	{
		ReadData =OTP8K_ReadByte(i);
		FMWriteOne(Addr_Flash_ROMStart+i,ReadData);
	}

	//Verify data
	ResultFlag = OTP8K_Verify();
	//power off
	POWER_OFF(vpp00,vdd00);
	Delay_1ms(50);

	return ResultFlag;
}
//test code
//void OTP_Test()
//{
//	u8 readdata[30];
//	u8 i;
//	if(OTP_Mode_IN(ID_M10P02R,vpp12,vdd65)==1)
//	{
//		LEDOK_On;
//	}
//	else
//		LEDOK_Off;
//	
//	 //testPower();
//	 
//	 // readdata=OTP_ReadByte( 0x1800);
//	  Delay_1ms(1);
//	  for(i=0;i<30;i++)
//	  {
//		  OTP_WriteByte((0x1807+i),i);
//	  }
//	  for(i=0;i<30;i++)
//	  {
//		readdata[i]=OTP_ReadByte( 0x1807 +i);
//	  }
//	  POWER_OFF(vpp00,vdd00);
//	  Delay_1ms(100);
//		  if(readdata[0]==0xaa)
//		  {
//			LEDOK_Off;
//			
//		  }
//	
//}