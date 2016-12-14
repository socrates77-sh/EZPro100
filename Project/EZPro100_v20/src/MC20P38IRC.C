
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "global.h"
#include "erorrNum.h"
#include "spi.h"
#include "OTPRW.h"
#include "power.h"
#include "IRC.h"
#include "M201IRC.H"
#include "MC224BPro.h"
#include "MC20P38IRC.H"
#include "MC20P38Pro.H"
#include "Config.h"
#include "OTPRW_8K.h"

u16 const  M238_Addr0[9]={0x3ffe,0x3ffe,0x3ffe,0x3ffe,0x3ffe,0x3fff,0x2c00,0x2c00,0x2c01};
u8  const  M238_Data0[9]={0x2c,  0x2c,  0x2c,  0x2c,  0x2c,  0x00,  0xa6,  0xa6,  0xff};

u16 const  M238_Addr1[4]={0x2c02,0x2c03,0x2c03,0x2c00};
u8  const  M238_Data1[4]={0x20,  0xfc,  0xfc,  0xa6};

 u16 const  M106_Addr0[27]={0x3ffe,0x3ffe,0x3ffe,0x3ffe,0x3ffe,0x3fff,0x3c00,0x3c00,0x3c01,0x3c02,0x3c03,0x000c,0x000c,0x3c04,0x3c05,0x3c06,0x3c07,0x000b,
 0x000b,0x3c08,0x3c09,0x3c0a,0x3c0b,0x000d,0x000d,0x3c0c,0x3c0d};

 u8  const  M106_Data0[27]={0xfc,0xfc,0xfc,0xfc,0xfc,0x00,0xa6,0xa6,0x10,0xb7,0x0c,0xff,0xff,0xa6,0x08,0xb7,0x0b,0xff,0xff,0xa6,0x46,0xb7,0x0d,0xff,0xff,
 0xa6,0x80};

 u16 const  M106_Addr1[4]={0x3c0e,0x3c0f,0x3c0f,0x3c0c};
 u8  const  M106_Data1[4]={0x20,  0xfc,  0xfc,  0xa6};



////IRC mainfuntion
u8 M238IRC(u16 OTPID,u8 IRC_Option,u16 IRC_Addr,u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{

	u8 i;
	u8 ResultFlag;

	POWER_ON(vpp12,vdd65);
	Delay_1ms(10);
	OP_STATE=0; 
	OTP8K_Mode_s(OTPID);	//Write OTP ID

	//send option data
	M238OPRATION(0x3ffe,IRC_Option);

	//vpp pull down to vdd
	//
	POWER_ON(vpp00,vdd30);
	Delay_1ms(3);
    if (DeviceConfig_xx.MCU_ID==0x0106)
    {
		for(i=0;i<27;i++)
		{
			ResultFlag=M238OPRATION(M106_Addr0[i],M106_Data0[i]);
			if(ResultFlag == 0)
			{	
				ERORR_VALUE=IRC_ModeIn_false;
				return 0;
			}		
		}
    } 
    else
    {
		for(i=0;i<9;i++)
		{
			ResultFlag=M238OPRATION(M238_Addr0[i],M238_Data0[i]);
			if(ResultFlag == 0)
			{	
				ERORR_VALUE=IRC_ModeIn_false;
				return 0;
			}		
		}
    }


	//test freq
	if(M238FTest(IRC_Option,IRC_Addr,IRC_FreqMin,IRC_FreqMax,IRC_FreqType)!=1) 
	{
		//LEDNG_On;
		//LEDOK_Off;
		POWER_OFF(vpp00,vdd00);
		ConfigTIM4();
		Delay_1ms(200);
		ERORR_VALUE=IRC_Value_false;
		return 0;
	}
	else
	{
		POWER_OFF(vpp00,vdd00);
		ConfigTIM4();
		Delay_1ms(200);
		return 1;
	}


}


u8 M238OPRATION(u16 ADDRR,u8 SendData)
{
	u8 i;
	u16 addr=0;
	u16 redata=0;
	u8  temp=0;

	// IRC adjust
	OTP_SDI_1;

	OTP_SCK_0; //0
	NOP;
	OTP_SCK_1;
	NOP;

	for(i=14;i>0;i--)
	{
		OTP_SCK_0;
		addr=GPIO_ReadInputDataBit(OTP_SDO_PORT,OTP_SDO_PIN);
		redata |= addr<<(i-1);
		NOP;
		OTP_SCK_1;//D7
		NOP;
	}
	if (redata != ADDRR)
	{

		return 0;
	}
	OTP_SCK_0; //sck 14
	OTP_SDI_1;
	NOP;
	OTP_SCK_1;
	NOP;

	OTP_SCK_0; //sck 15
	NOP;
	OTP_SCK_1;
	NOP;
	OTP_SCK_0; //sck 16
	NOP;
	OTP_SCK_1;
	NOP;

	for(i=8;i>0;i--)
	{
		OTP_SCK_0;
		temp = SendData >>(i-1);
		if ((temp & 0x01)==1)
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
	OTP_SDI_0;

	OTP_SCK_0; //25
	NOP;
	OTP_SCK_1;
	NOP;
	OTP_SCK_0; //26
	NOP;
	OTP_SCK_1;
	NOP;
	OTP_SCK_0; //27
	NOP;
	OTP_SCK_1;
	NOP;

	return 1;
}


//irc
u8 M238FTSendData(u8 IRC_Option, u16 AAdjust,u8 DAdjust)
{
	u8 i;
	u8 result=0;
	u8 temp;
	u16 b=0;
	u16 c=0;

	u16 cnt[3];
	//u16 cnt_a;

	for(i=0;i<4;i++)
	{
		if (DeviceConfig_xx.MCU_ID==0x0106)
		{
			result=M238OPRATION(M106_Addr1[i],M106_Data1[i]); //M224B_Data1[i] is the same as M201
		} 
		else
		{
			result=M238OPRATION(M238_Addr1[i],M238_Data1[i]); //M224B_Data1[i] is the same as M201
		}
		
		if (result==0)
		{
			return 0;
		}
	}

	// IRC adjust
	OTP_SDI_1;

	OTP_SCK_0; //0
	NOP;
	OTP_SCK_1;
	NOP;

	for(i=14;i>0;i--)
	{
		OTP_SCK_0;
		b=GPIO_ReadInputDataBit(OTP_SDO_PORT,OTP_SDO_PIN);
		c |= b<<(i-1);
		NOP;
		OTP_SCK_1;//D7
		NOP;
	}
	if (c != AAdjust)
	{
		return 0;
	}
	OTP_SCK_0; //sck 14
	NOP;
	OTP_SCK_1;
	NOP;

	OTP_SCK_0; //sck 15
	NOP;
	OTP_SCK_1;
	NOP;
	OTP_SCK_0; //sck 16
	NOP;
	OTP_SCK_1;
	NOP;

	for(i=8;i>0;i--)
	{
		OTP_SCK_0;
		temp = DAdjust >>(i-1);
		if ((temp & 0x01)==1)
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
	OTP_SDI_0;

	OTP_SCK_0; //25
	NOP;
	OTP_SCK_1;
	NOP;
	OTP_SCK_0; //26
	NOP;
	OTP_SCK_1;
	NOP;
	OTP_SCK_0; //27_0
	NOP;

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

	OTP_SCK_1;
	NOP;

	return 1;

}



u8 M238FTest(u8 IRC_Option,u16 IRC_Addr,u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
	u8 mm=0x80;
	u8 i;
	OPTION_FRT=0; //initial
	if (DeviceConfig_xx.MCU_ID==0x0106)
	{
		IRC_Addr=0x3c0d;
	}
	for(i=0;i<8;i++)
	{
		//mm=0x80;
		M238FTSendData(IRC_Option,IRC_Addr,mm);
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

	//
	M238FTSendData(IRC_Option,IRC_Addr,mm);
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