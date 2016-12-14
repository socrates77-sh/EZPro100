
#include "stm32f10x.h"
#include "stm32f10x_dma.h"
#include "erorrNum.h"
#include "Config.h"
#include "global.h"
#include "spi.h"
#include "OTPRW.h"
#include "power.h"
#include "MC224BPro.h"

#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "platform_config.h"
#include "usb_pwr.h"
#include "stm32_eval.h"
#include "LCD.h"
#include "wx_i2c.h"
#include "IRC.h"

u16 IRC_VALUE;
u16 TEST_IRC[100];
u16 TEST_COUNT;
u16 OTP_ADDR;
u32 OKcounter;
u16 NGcounter;
u8  WR_Command; // write ? read ? verify ? blank? 
u16 USB_RxDataCS;
u8  USB_RxCommand;
u8  OPTION_FRT;
u8  OPTION_FRT2;
u8  OPTION_FRT3;
u8  OPTION_TEMPADJ;
u16 OPTION_TEMP;
u8  IRC_MODEL;
u8  OTP_ReadData;
u8  ERORR_VALUE; //Return error number
u16 adc_value[3];
u8 USB_Rx_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
u8 USB_Tx_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
u8 USB_Rx_Flag;
u8 StateFlag=0xfe;
u8 ModelIn_result=1;
u8 iic_data[IIC_DATA_BUFF_SIZE];
u8 OP_STATE;
u16 OSCCAL;//校准得到的频率校准值
u8  TADJ;//校准得到的温度校准值

u8 WriteFlag;//=1 mc32t8132在线烧录；=0 mc32t8132裸片烧录

/*
typedef enum {

	LED1 = 0,
	LED2,
	LED3,
	LED4,
	LED5,

}LED_STATE;

LED_STATE led_status;

**/

/**

* @brief  Configures the different system clocks.

* @param  None

* @retval : None

*/

void RCC_Configuration(void)

{

	/* Setup the microcontroller system. Initialize the Embedded Flash 

	Interface, initialize the PLL and update the SystemFrequency variable. */

	SystemInit();

	/* GPIOA clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);

	//mco initial
	/*
		RCC_MCO_NoClock----无时钟输出
		RCC_MCO_SYSCLK-----输出系统时钟
		RCC_MCO_HSI--------输出内部高 8MHZ
		RCC_MCO_HSE--------输出高速外部时钟信号
		RCC_MCO_PLLCLK_DIV2--输出 PLL倍频后的二分频时钟
	*/
	RCC_MCOConfig(RCC_MCO_NoClock);
	//RCC_MCOConfig(RCC_MCO_PLLCLK_Div2);

}

/**

* @brief  Configure the GPIOD Pins.

* @param  None

* @retval : None

*/

void GPIO_Configuration(void)

{

	GPIO_InitTypeDef GPIO_InitStructure;

	/* GPIOF configuration: PC6 ->OSCO(CP),PC7 ->OSCI(TIME_REF), */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11 |GPIO_Pin_12;

	//GPIO_InitStructure.GPIO_Pin = ; //PC11->LEDNG,PC12 ->LEDBUSY

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; // 开漏输出

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_13; //pc13 as CLN_VDDtoGND
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
        //CP_0;
        VDD_IO_Off;
        VPP_IO_Off;
	CLN_VDD2GND_Off;

	// select KEY
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU; // input push up
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_2MHz;

	GPIO_Init(GPIOD,&GPIO_InitStructure);

	//PA8 config as MCO CLK output
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	//PB1 as USB pushup
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	//PB5=IIC_SDA, PB6=IIC_SCL
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

    USB_REG_On;

	//VDD_ADC= PA1/ADC1,VPP_ADC=PA2/ADC2,ADJ_ADC=PA3/ADC3

	

}

void Freq(u8 Numb)
{
	if (Numb==1)
	{
		LEDNG_Off;
		LEDOK_On;
		GPIO_SetBits(GPIOA,GPIO_Pin_10);
		//Delay_1ms(30);
        //OKcounter=OKcounter+1;
		clear_line(4); //clear Write & Check
		display_OK_NG(OKcounter,1);

		GPIO_ResetBits(GPIOA,GPIO_Pin_10);

		display_OK_NG(NGcounter,0);
		clear_line(6); //clear Write & Check
		display_dynamicID();	
	} 
	else
	{
		LEDOK_Off;
		LEDNG_On;
		GPIO_SetBits(GPIOA,GPIO_Pin_10);
		Delay_1ms(10);
		clear_line(4);
		GPIO_ResetBits(GPIOA,GPIO_Pin_10);
		//Delay_1ms(20);
		displayErorr(ERORR_VALUE);
		GPIO_SetBits(GPIOA,GPIO_Pin_10);
		//Delay_1ms(20);
		display_Addr();
		GPIO_ResetBits(GPIOA,GPIO_Pin_10);

	}
}

void Delay_100us(u16 cnt )
{
	u16 i,a;
	
	for (a=0 ;a<cnt;a++)
	{
		for(i=800;i>0;i--);
	}

}
void Delay_10us(u16 cnt )//徐明明加 根据Delay_100us改的  并没有用示波器测量 可能不准
{
	u16 i,a;
	
	for (a=0 ;a<cnt;a++)
	{
		for(i=100;i>0;i--);
	}

}
void Delay_1us(u16 cnt )//徐明明加 根据Delay_100us改的  并没有用示波器测量 可能不准
{
	u16 i,a;
	
	for (a=0 ;a<cnt;a++)
	{
		for(i=40;i>0;i--);
	}

}
void Delay_1ms(u16 cnt )
{
	u16 i,a;

	for (a=0 ;a<cnt;a++)
	{
		for(i=8500;i>0;i--);
	}

}

void DeviceConfig()
{
	u8 configdata[50];
	u8 tempcount;

    if (FMReadOne(0x018d00)==1)
    {
        tempcount =Rxdata;
    }


	FM_CS_0;
    if (tempcount==0xff)
    {
        FMReadMore(Addr_Flash_Config-1);
    } 
    else
    {
        FMReadMore(0x018d00-1);
    }

	for (tempcount=0;tempcount<=53;tempcount++)
	{
		SPI_ReadByte();
		configdata[tempcount]=Rxdata;
	}
	FM_CS_1;
	WR_Command = configdata[0];
	DeviceConfig_xx.MCU_ID= configdata[1]*256+configdata[2]; //[4]
	DeviceConfig_xx.ProgramID=configdata[3];
	DeviceConfig_xx.RomFirAddr=configdata[4]*256 + configdata[5];
	DeviceConfig_xx.RomEndAddr=configdata[6]*256 + configdata[7]; 
	DeviceConfig_xx.OptionAddr=configdata[8]*256 +configdata[9];
	DeviceConfig_xx.OptionSize=configdata[10];
	DeviceConfig_xx.OptionProCnt=configdata[11];
	DeviceConfig_xx.BlankCode =configdata[12];

	DeviceConfig_xx.IRC_OPTION=configdata[13];
	DeviceConfig_xx.IRC_FrMax=configdata[14]*256 + configdata[15];
	DeviceConfig_xx.IRC_FrMin=configdata[16]*256 + configdata[17];
	DeviceConfig_xx.IRC_FrType=configdata[18]*256 + configdata[19];

	DeviceConfig_xx.ProTime = configdata[20]*256 + configdata[21];
	DeviceConfig_xx.ProVDD =configdata[22]*256 + configdata[23]; //6.5V
	DeviceConfig_xx.ProVPP =configdata[24]*256 + configdata[25]; // 150
	DeviceConfig_xx.IrcVDD =configdata[26]*256 + configdata[27]; // 5.0=51 3.0=25
	DeviceConfig_xx.IrcVPP =configdata[28]*256 + configdata[29];
	DeviceConfig_xx.VerifyVDD=configdata[30]*256 + configdata[31];
	DeviceConfig_xx.VerifyVPP=configdata[32]*256 + configdata[33];

	DeviceConfig_xx.IRC_Select =configdata[34];
	DeviceConfig_xx.IRC_VALUE_ADDR =configdata[35]*256+configdata[36];
	DeviceConfig_xx.IRC_Shift_Value =configdata[37];
	DeviceConfig_xx.WriteEn =configdata[38];
	DeviceConfig_xx.MCU_Type=configdata[39];

	DeviceConfig_xx.MCU_Name[0]=configdata[40];  
	DeviceConfig_xx.MCU_Name[1]=configdata[41];
	DeviceConfig_xx.MCU_Name[2]=configdata[42];
	DeviceConfig_xx.MCU_Name[3]=configdata[43];
	DeviceConfig_xx.MCU_Name[4]=configdata[44];
	DeviceConfig_xx.MCU_Name[5]=configdata[45];
	DeviceConfig_xx.MCU_Name[6]=configdata[46];
	DeviceConfig_xx.MCU_Name[7]=configdata[47];
	DeviceConfig_xx.MCU_Name[8]=configdata[48];
	DeviceConfig_xx.MCU_Name[9]=configdata[49];

        if (FMReadOne(0x018d70)==1)
        {
            tempcount =Rxdata;
        }
        if (tempcount==0x39)
        {
            return;
        }
	if (FMReadOne(0x017d70)==1)
	{
		tempcount =Rxdata;
	}
	
	if (tempcount!=0x39)
	{
		ERORR_VALUE=VS_Low;
		IIC_Write(0,0xff);
		IIC_Write(0,0xff);
	}


}