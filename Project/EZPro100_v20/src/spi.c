
#include "stm32f10x.h"
#include "global.h"
#include "spi.h"
#include "OTPRW.h"
#include "power.h"
#include "IRC.h"
#include "MC224BPro.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "Config.h"

u8 Rxdata;
//u8 EEORE;

void SPI2Init(void)
{
	SPI_InitTypeDef   SPI_InitSructure;
	GPIO_InitTypeDef  GPIO_IniStructure;

	//initial SPI CLK
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);

	//define pin for spi2 as AF
	GPIO_IniStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 ;
	GPIO_IniStructure.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_IniStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_IniStructure);

	GPIO_IniStructure.GPIO_Pin = GPIO_Pin_12; // pin_11-->MCP42050_CS
	GPIO_IniStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_IniStructure.GPIO_Mode =GPIO_Mode_Out_OD; // FLASH CE PIN
	GPIO_Init(GPIOB,&GPIO_IniStructure);
    FM_CS_1;
	GPIO_IniStructure.GPIO_Pin = GPIO_Pin_11; // pin_11-->MCP42050_CS
	GPIO_IniStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_IniStructure.GPIO_Mode =GPIO_Mode_Out_PP; // FLASH CE PIN
	GPIO_Init(GPIOB,&GPIO_IniStructure);

	MCP42050_CS_1;
	//config spi2
	SPI_InitSructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
	SPI_InitSructure.SPI_CPHA = SPI_CPHA_2Edge; //CLK = 1 when idel mode
	SPI_InitSructure.SPI_CPOL = SPI_CPOL_High;  // sample data when push
	SPI_InitSructure.SPI_CRCPolynomial = 7; //?
	SPI_InitSructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitSructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitSructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitSructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitSructure.SPI_NSS = SPI_NSS_Soft;

	SPI_Init(SPI2,&SPI_InitSructure);
	SPI_Cmd(SPI2,ENABLE);

}


//返回读写是否成功标记。 0失败，1成功。
u8 SPI_WriteByte(u8 TxData)
{
	u16 cw=0;

	//等待发送缓冲区空
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
	{
		if(++cw > 60000) return 0;
	}
	//发一个字节
	SPI_I2S_SendData(SPI2, TxData);
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
	{
		if(++cw > 60000) return 0;
	}
	SPI_I2S_ReceiveData(SPI2);

	return 1;
}

//
u8 SPI_ReadByte( )
{
	u16 cr=0;
	//u8 Rxdata;

	//等待数据接收
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
	{
		if(++cr > 60000)return 0;   
	}
    // send clk 
	SPI_I2S_SendData(SPI2,0xaa);
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) ==RESET)
	{
		if(++cr > 60000) return 0;   
	}
	//取数据
	Rxdata = SPI_I2S_ReceiveData(SPI2);

	return 1;
}

/************************************************
//MCP42050 ADJUST VDD
//
//
**************************************************/
void MCP42050_ADJ(u8 PotX,u8 Rvalue)
{
	MCP42050_CS_0;
	SPI_WriteByte(PotX); //Potentiometer 1=0x12; Potentiometer 2=0x11
	SPI_WriteByte(Rvalue);
	MCP42050_CS_1;
	Delay_100us(1);
}


/************************************************************
//FuntionName:
//Input		 :
//Output     :
//
***********************************************************/
void FMWriteOne(u32 FMAddr,u8 FMData)
{
	//u8 temp=3;
	u16 i=0;

	FM_CS_0 ;
        SPI_WriteByte(WRENCMD);
	FM_CS_1 ;
        while(i<200) i++;
	FM_CS_0;
	SPI_WriteByte(ByteProgramCMD);
	SPI_WriteByte((u8)(FMAddr >> 16));
	SPI_WriteByte((u8)(FMAddr>>8));
	SPI_WriteByte((u8)(FMAddr & 0x0000ff));
	SPI_WriteByte(FMData);
	FM_CS_1;
}

u8 FMReadOne(u32 FMAddr)
{
  u8 temp	=0;

  FM_CS_0;
  SPI_WriteByte(ReadCMD);
  SPI_WriteByte((u8)(FMAddr >> 16));
  SPI_WriteByte((u8)(FMAddr>>8));
  SPI_WriteByte((u8)(FMAddr & 0x0000ff));
  if (SPI_ReadByte()==1)
  {
    temp =1;
  }
  else
  {
    temp= 0;
  }
  FM_CS_1;
  return temp;
  // data output stream is continuous through all addresses 
  // until CE# from low to high
}


void	FMChipErase()
{
	u8 i=0;
        //u8 temp;
        
	FM_CS_0 ;
	SPI_WriteByte(WRENCMD);
	FM_CS_1 ;
	while(i<200) i++;
	i=0;

	FM_CS_0;
	SPI_WriteByte(ChipEraseCMD);
	while(i<200) i++;
	FM_CS_1;
	while((FMReadStatus()&& 0x01)==1) ;

}

u8 FMReadStatus()
{	
	u8 xdata;

	FM_CS_0;
	SPI_WriteByte(RDSRCMD);
	if (SPI_ReadByte()==1)
	{
		xdata = Rxdata;
	}
	
	FM_CS_1;
	return xdata;

}

void FMWriteStatus(u8 BPdata )
{
	u8 i=0;
	FM_CS_0;
	SPI_WriteByte(EWSRCMD);
	FM_CS_1;
	while(i<200) i++;
	FM_CS_0;
	SPI_WriteByte(WRSRCMD);
	SPI_WriteByte( BPdata ); //clear BP0,BP1
	FM_CS_1;

}

u8 FMReadID()
{
	u8 mfid=0;
	u8 dvid=0;

	FM_CS_0;
	SPI_WriteByte(ReadIDCMD);
	SPI_WriteByte(0x00);
	SPI_WriteByte(0x00);
	SPI_WriteByte(0x00);
	//mfid=SPI_ReadByte();
	//dvid=SPI_ReadByte();
	if (SPI_ReadByte() ==1)
	{
		mfid=Rxdata;
	}
	if (SPI_ReadByte()==1)
	{
		dvid=Rxdata;
	}
	FM_CS_1;
        if(mfid == 0xbf)
          if(dvid == 0x49)
            return 1;
          else
            return 0;
         else 
           return  0;
}



//Raed more bytes 
u8 FMReadMore(u32 FirstAddr)
{
	u8 temp	=0;

	SPI_WriteByte(ReadCMD);
	SPI_WriteByte((u8)(FirstAddr >> 16));
	SPI_WriteByte((u8)(FirstAddr>>8));
	SPI_WriteByte((u8)(FirstAddr & 0x0000ff));

	if (SPI_ReadByte()==1)
	{
		temp =1;
	}
	else
	{
		temp= 0;
	}
	return temp;
}

u16 CalculateCheckSum()
{
	u16 checksum=0;
	u16 addr0;
	u16 addr1;
    if (DeviceConfig_xx.MCU_ID==0x3316)
    {
        if(FMReadOne(Addr_Flash_CfgFirADD0_16k))
            addr0=Rxdata;
        if (FMReadOne(Addr_Flash_CfgFirADD1_16k))
        {
            addr1=Rxdata;
        }
    }
    else
    {
        if(FMReadOne(Addr_Flash_CfgFirADD0))
            addr0=Rxdata;
        if (FMReadOne(Addr_Flash_CfgFirADD1))
        {
            addr1=Rxdata;
        }
    }

	
	DeviceConfig_xx.RomFirAddr=addr0*256+addr1;
    if (DeviceConfig_xx.RomFirAddr==0xffff)
    {
		return 0;
    }
    if (DeviceConfig_xx.MCU_ID==0x3316)
    {
        addr0=FMReadOne(Addr_Flash_CfgFirADD2_16k);
        addr0=Rxdata;
        addr1=FMReadOne(Addr_Flash_CfgFirADD3_16k);
        addr1=Rxdata;
    } 
    else
    {
        addr0=FMReadOne(Addr_Flash_CfgFirADD2);
        addr0=Rxdata;
        addr1=FMReadOne(Addr_Flash_CfgFirADD3);
        addr1=Rxdata;
    }

	DeviceConfig_xx.RomEndAddr=addr0*256+addr1;
	if (DeviceConfig_xx.RomEndAddr==0x0000)
	{
		return 0;
	}

	FM_CS_0;
	FMReadMore(Addr_Flash_ROMStart+DeviceConfig_xx.RomFirAddr-1);
	for(OTP_ADDR=DeviceConfig_xx.RomFirAddr;OTP_ADDR<=DeviceConfig_xx.RomEndAddr;OTP_ADDR++)
	{
		SPI_ReadByte();
		checksum +=Rxdata;			
	}
	FM_CS_1;

	return checksum;
}

/* test 
//funtion test 
void SPI_Test()
{
	
	FMReadID();
	FMReadStatus();
	FMWriteStatus(BP_MemoryArray0); 
	FMReadStatus();
	FMChipErase();    
	FMWriteOne(0x1800,0xaa);
	FMWriteOne(0x1801,0x55)
	FMWriteOne(0x1802,0x01);
	FMWriteOne(0x1803,0x02);

	u8 data[4]={0,0,0,0};
	u16 cnt=0;


	FM_CS_0;
	FMReadMore(0x1800);
	data[0]=Rxdata;
	SPI_ReadByte();
	data[1]=Rxdata;
	SPI_ReadByte();
	data[2]=Rxdata;
	SPI_ReadByte();
	data[3]=Rxdata;
	FM_CS_1;
	
	FMReadOne(0x1800);
	data[0]=Rxdata;
	FMReadOne(0x1801);
	data[1]=Rxdata;
	FMReadOne(0x1802);
	data[2]=Rxdata;
	FMReadOne(0x1803);
	data[3]=Rxdata;
	
	
	if((data[0]==0xaa)&&(data[1]==0x55)&&(data[2]==0x01)&&(data[3]==0x02))
	{ 
		GPIO_ResetBits(LEDOK) ; //LEDD1=0
	}
	else
	{
		GPIO_SetBits(LEDOK);
	}
}

*/