
/* Includes ------------------------------------------------------------------*/

#include "stm32f10x.h"

#include "stm32f10x_tim.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "global.h"
#include "spi.h"
#include "power.h"
#include "OTPRW.h"
#include "IRC.h"
#include "M201IRC.h"
#include "MC224BPro.h"
#include "Config.h"
#include "ADC.h"
#include "OTPRW_8K.h"
#include "MC20P38Pro.h"

#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"

#include "USB_Rx_Tx.h"
#include "LCD.h"
#include "wx_i2c.h"
#include "MC301Pro.h"
#include "MC32P21Pro.h"
#include "MC30P6060.h"
#include "MC32P7011.h"
#include "MC32T8132.h"
#include "MC32P5222Pro.h"

u8 const ProgrammingFinished[5]={0x68,0x01,0x02,0x03,0x16};
u8 const BlankOk[5]={0x68,0x01,0x04,0x05,0x16};
u8 const NextData[5]={0x68,0x01,0x12,0x13,0x16};
u8 const Reset[5]={0x68,0x01,0xe2,0xe3,0x16};
//u8 const MotherChipMade[5]={0x68,0x01,0x06,0x07,0x16};
u8 const Reconfigured[5]={0x68,0x01,0x22,0x23,0x16};
u8 const EraseFlash[5]={0x68,0x01,0x08,0x09,0x16};
u8 const ModelIn[5]={0x68,0x01,0x36,0x37,0x16};

//u8 const ReadVison[7]={0x68,0x03,0x46,0x01,0x01,0x4b,0x16};

u8 const ErrorsCS[6]={0x68,0x02,0xf2,0x05,0xf9,0x16};

void USB_Rx_Tx()
{
  u32 i,temp;
  u16 Read_otp_addr;
  u8 checksum=0;
  u8 reFlag=0;
  u16 m301data=0;
  //u32 
  //checksum
  for(i=1;i<USB_Rx_Buffer[1]+2;i++)
  {
    checksum +=USB_Rx_Buffer[i];
  }
  if (checksum != USB_Rx_Buffer[i])
  {
    //error
    for(temp=0;temp<6;temp++)
    {
      USB_Tx_Buffer[temp]=ErrorsCS[temp];
    }
    USB_SendData(6);
    LEDNG_On;
    LEDBUSY_Off;
    LEDOK_Off;
    Freq(0);
  }
  else if (USB_Rx_Buffer[2]==0x01)
  {
    //programming
    StateFlag =0xf7; //Writer otp
    USB_RxCommand=0x01;
    //for(temp=0;temp<5;temp++)
    //{
    //	USB_Tx_Buffer[temp]=ProgrammingFinished[temp];
    //}
    //USB_SendData(5);
  }
  else if (USB_Rx_Buffer[2]==0x45)//Read vision
  {
    USB_Tx_Buffer[0]=0x68;
    USB_Tx_Buffer[1]=0x06;
    USB_Tx_Buffer[2]=0x46;
    USB_Tx_Buffer[3]=2;//Vision: v2.94
    USB_Tx_Buffer[4]=96;
    USB_Tx_Buffer[5]=16;//Date: 
    USB_Tx_Buffer[6]=6;
    USB_Tx_Buffer[7]=17; 
    USB_Tx_Buffer[8]=0x4b + USB_Tx_Buffer[3] +USB_Tx_Buffer[4]+USB_Tx_Buffer[5]+USB_Tx_Buffer[6]+USB_Tx_Buffer[7];
    USB_Tx_Buffer[9]=0x16;
    USB_SendData(10);

  }
  else if (USB_Rx_Buffer[2]==0x03) ////blank
  {
    if (DeviceConfig_xx.ProgramID==0x03)
    {
      reFlag=OTP8K_Blank(DeviceConfig_xx.RomFirAddr,DeviceConfig_xx.RomEndAddr,DeviceConfig_xx.OptionAddr);
    } 
    else
    {
      reFlag=OTP_Blank(DeviceConfig_xx.RomFirAddr,DeviceConfig_xx.RomEndAddr,DeviceConfig_xx.OptionAddr);
    }		
    LEDBUSY_Off;
    if(reFlag ==1)
    {
      for(temp=0;temp<5;temp++)
      {
        USB_Tx_Buffer[temp]=BlankOk[temp];
      }
      USB_SendData(5);
      LEDNG_Off;
      //LEDBUSY_Off;
      LEDOK_On;
      Freq(1);
    }
    else
    {
      USB_Tx_Buffer[0]=0x68;
      USB_Tx_Buffer[1]=0x06;
      USB_Tx_Buffer[2]=0xf2;
      USB_Tx_Buffer[3]=0x03;
      USB_Tx_Buffer[4]=OTP_ADDR/0x100;
      USB_Tx_Buffer[5]=OTP_ADDR%0x100;
      USB_Tx_Buffer[6]=0xaa; //Rxdata;  //Flash data
      USB_Tx_Buffer[7]=OTP_ReadData;
      USB_Tx_Buffer[8]=0xfb + USB_Tx_Buffer[4] +USB_Tx_Buffer[5]+USB_Tx_Buffer[6]+USB_Tx_Buffer[7];
      USB_Tx_Buffer[9]=0x16;
      USB_SendData(10);
      LEDNG_On;
      //LEDBUSY_Off;
      LEDOK_Off;
      Freq(0);
    }
  }
  else if (USB_Rx_Buffer[2]==0x04) //write data to eeprom 24c02
  {		
    /*IIC_Write(0x01,0x55);
    OKorNG=IIC_Read(0x01);
    IIC_Write_Array(testdata,0x0001,5);    
    IIC_Read_Array(testdata,0x0001,5);*/
    //temp=USB_Rx_Buffer[4]; //eeprom addr 
    for (i=0;i<32;i++)
    {
      IIC_Write(i,USB_Rx_Buffer[5+i]);
    }
    //IIC_Write_Array(iic_data,0x0000,IIC_DATA_BUFF_SIZE);

    //verify
    USB_Tx_Buffer[2]=0x5a;
    for(temp=0;temp<32;temp++)
    {
      i=IIC_Read(temp);
      if (i!=USB_Rx_Buffer[temp+5])
      {
        USB_Tx_Buffer[2]=0x0a;
        break;
      }
    }
    USB_SendData(1);
  }
  else if (USB_Rx_Buffer[2]==0x05) //make mother chip
  { 
    reFlag=FMReadOne(0x018d01); //read mc33p116 id
    checksum=Rxdata;
    if (checksum!=0xff) //mcu=mc33p116
    {
      reFlag=FMReadOne(0x018d02); //read mc33p116 id
      reFlag=Rxdata;
    } 
    else
    {
      reFlag=FMReadOne(Addr_Flash_MCU_ID);
      checksum=Rxdata;
      reFlag=FMReadOne(Addr_Flash_MCU_ID+1);
      reFlag=Rxdata;
    }
    DeviceConfig_xx.MCU_ID=checksum*0x100+reFlag;
    //make mother chip
    USB_RxDataCS=CalculateCheckSum();
    if(DeviceConfig_xx.MCU_ID==0x3081||DeviceConfig_xx.MCU_ID==0x6060)
    {
      USB_RxDataCS &= 0x3fff;
    }
    else if(DeviceConfig_xx.MCU_ID==0x3401)
    {
      USB_RxDataCS &= 0x1fff;
    }
    if ((DeviceConfig_xx.MCU_ID==0x0238)||(DeviceConfig_xx.MCU_ID==0x3378)||(DeviceConfig_xx.MCU_ID==0x3394)
            ||(DeviceConfig_xx.MCU_ID==0x0278)||(DeviceConfig_xx.MCU_ID==0x0268)) 
    {
      reFlag=FMReadOne(Addr_Flash_OptionProtec8k);
      USB_Tx_Buffer[5]=Rxdata;
      reFlag=FMReadOne(Addr_Flash_OptionProtec8k+1);
      USB_Tx_Buffer[6]=Rxdata;
      reFlag=FMReadOne(Addr_Flash_OptionProtec8k+2);
      USB_Tx_Buffer[7]=Rxdata;
      reFlag=FMReadOne(Addr_Flash_OptionProtec8k+3);
      USB_Tx_Buffer[8]=Rxdata;
    }
    else if(DeviceConfig_xx.MCU_ID==0x3316)
    {
      reFlag=FMReadOne(addr_Flash_OptionProtec16K);
      USB_Tx_Buffer[5]=Rxdata;
      reFlag=FMReadOne(addr_Flash_OptionProtec16K+1);
      USB_Tx_Buffer[6]=Rxdata;
      reFlag=FMReadOne(addr_Flash_OptionProtec16K+2);
      USB_Tx_Buffer[7]=Rxdata;
      reFlag=FMReadOne(addr_Flash_OptionProtec16K+3);
      USB_Tx_Buffer[8]=Rxdata;
    }
    else
    {
      reFlag=FMReadOne(Addr_Flash_OptionProtec);
      USB_Tx_Buffer[5]=Rxdata;
      reFlag=FMReadOne(Addr_Flash_OptionProtec+1);
      USB_Tx_Buffer[6]=Rxdata;
      reFlag=FMReadOne(Addr_Flash_OptionProtec+2);
      USB_Tx_Buffer[7]=Rxdata;
      reFlag=FMReadOne(Addr_Flash_OptionProtec+3);
      USB_Tx_Buffer[8]=Rxdata;
    }
    USB_Tx_Buffer[0]=0x68;
    USB_Tx_Buffer[1]=0x07;
    USB_Tx_Buffer[2]=0x06;
    USB_Tx_Buffer[3]=USB_RxDataCS/0x100; //check_sum
    USB_Tx_Buffer[4]=USB_RxDataCS%0x100;
    USB_Tx_Buffer[9]=0x0a+USB_Tx_Buffer[3]+USB_Tx_Buffer[4]+USB_Tx_Buffer[5]+USB_Tx_Buffer[6]+USB_Tx_Buffer[7]+USB_Tx_Buffer[8];
    USB_Tx_Buffer[10]=0x16;
    USB_SendData(11);
    DeviceConfig();
    for(temp=0;temp<IIC_DATA_BUFF_SIZE;temp++) // dynamic id
    {
      iic_data[temp]=IIC_Read(temp);
    }
    Initial_Lcd();
    displayfuntion();//LCD 
    LEDNG_Off;
    LEDBUSY_Off;
    LEDOK_On;   
    Freq(1);
  }
  else if (USB_Rx_Buffer[2]==0x06) //read eeprom data
  {
    for(temp=0;temp<32;temp++)
    {
      USB_Tx_Buffer[temp]=IIC_Read(temp);
    }
    USB_SendData(32);
  }
  else if (USB_Rx_Buffer[2]==0x07) //erase flash
  {
    FMWriteStatus(BP_MemoryArray0); //clear protect of all
    FMChipErase();
    for(temp=0;temp<5;temp++)
    {
       USB_Tx_Buffer[temp]=EraseFlash[temp];
    }
    USB_SendData(5);         
  }
  else if (USB_Rx_Buffer[2]==0x11) //download data 
  {
    temp=USB_Rx_Buffer[3]*256+USB_Rx_Buffer[4];
    if(temp == 0x2100)
    {
      USB_Rx_Buffer[4] = temp;
    }
    else if(temp == 0x8000)
    {
      USB_Rx_Buffer[4] = temp;
    }

    //for(i=0;i<16;i++)
    for( i=0; i<(USB_Rx_Buffer[1]-3) ; i++)
    {
      //ReadData =OTP_ReadByte(i);
      FMWriteOne(Addr_Flash_ROMStart+temp+i,USB_Rx_Buffer[i+5]);
    }
    for(temp=0;temp<5;temp++)
    {
      USB_Tx_Buffer[temp]=NextData[temp];
    }
    USB_SendData(5);
  }	
  else if (USB_Rx_Buffer[2]==0x21)
  {
    WR_Command = USB_Rx_Buffer[5];
    DeviceConfig_xx.MCU_ID= USB_Rx_Buffer[6]*256+USB_Rx_Buffer[7]; //[4]
    DeviceConfig_xx.ProgramID=USB_Rx_Buffer[8];
    DeviceConfig_xx.RomFirAddr=USB_Rx_Buffer[9]*256 + USB_Rx_Buffer[10];
    DeviceConfig_xx.RomEndAddr=USB_Rx_Buffer[11]*256 + USB_Rx_Buffer[12]; 
    DeviceConfig_xx.OptionAddr=USB_Rx_Buffer[13]*256 +USB_Rx_Buffer[14];
    DeviceConfig_xx.OptionSize=USB_Rx_Buffer[15];
    DeviceConfig_xx.OptionProCnt=USB_Rx_Buffer[16];
    DeviceConfig_xx.BlankCode =USB_Rx_Buffer[17];
    DeviceConfig_xx.IRC_OPTION=USB_Rx_Buffer[18];
    DeviceConfig_xx.IRC_FrMax=USB_Rx_Buffer[19]*256 + USB_Rx_Buffer[20];
    DeviceConfig_xx.IRC_FrMin=USB_Rx_Buffer[21]*256 + USB_Rx_Buffer[22];
    DeviceConfig_xx.IRC_FrType=USB_Rx_Buffer[23]*256 + USB_Rx_Buffer[24];
    DeviceConfig_xx.ProTime = USB_Rx_Buffer[25]*256 + USB_Rx_Buffer[26];
    DeviceConfig_xx.ProVDD =USB_Rx_Buffer[27]*256 + USB_Rx_Buffer[28];
    DeviceConfig_xx.ProVPP =USB_Rx_Buffer[29]*256 + USB_Rx_Buffer[30];
    DeviceConfig_xx.IrcVDD =USB_Rx_Buffer[31]*256 + USB_Rx_Buffer[32];
    DeviceConfig_xx.IrcVPP =USB_Rx_Buffer[33]*256 + USB_Rx_Buffer[34];
    DeviceConfig_xx.VerifyVDD=USB_Rx_Buffer[35]*256 + USB_Rx_Buffer[36];
    DeviceConfig_xx.VerifyVPP=USB_Rx_Buffer[37]*256 + USB_Rx_Buffer[38];
    DeviceConfig_xx.IRC_Select =USB_Rx_Buffer[39];
    DeviceConfig_xx.IRC_VALUE_ADDR =USB_Rx_Buffer[40]*256 +USB_Rx_Buffer[41];
    DeviceConfig_xx.IRC_Shift_Value =USB_Rx_Buffer[42];
    DeviceConfig_xx.WriteEn =USB_Rx_Buffer[43];
    DeviceConfig_xx.MCU_Type=USB_Rx_Buffer[44];   
    //reConfig vpp,vdd value ---- 20t01 or otp or flash
    MCP42050_ADJ(ADJ_VPP,(u8)DeviceConfig_xx.ProVPP);//vpp value
    MCP42050_ADJ(ADJ_VDD,(u8)DeviceConfig_xx.IrcVDD); 
    for(temp=0;temp<5;temp++)
    {
      USB_Tx_Buffer[temp]=Reconfigured[temp];
    }
    USB_SendData(5);
  }
  else if (USB_Rx_Buffer[2]==0x37) //model in
  {
    //power on
    if (DeviceConfig_xx.ProgramID==0x03)
    {
      reFlag=OTP8K_Mode_IN(DeviceConfig_xx.MCU_ID,vpp12,vdd65);
    }
    else if(DeviceConfig_xx.ProgramID==0X02)
    {
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
      m301data=MC301_ReadByte(0X3FFF);
      if(m301data==DeviceConfig_xx.MCU_ID)
      { 
        reFlag=1;
      }
      else
      {reFlag=0;}
    }
    else if ((DeviceConfig_xx.ProgramID==0x04)||(DeviceConfig_xx.ProgramID==0x05))
    {
      /*
      if ((DeviceConfig_xx.MCU_ID==0x3394)||(DeviceConfig_xx.MCU_ID==0x3378)||(DeviceConfig_xx.MCU_ID==0X3111)||(DeviceConfig_xx.MCU_ID==0X9029)
        ||(DeviceConfig_xx.MCU_ID==0x3401)||(DeviceConfig_xx.MCU_ID==0x3316)||(DeviceConfig_xx.MCU_ID==0x5312)||(DeviceConfig_xx.MCU_ID==0x7022)
        ||(DeviceConfig_xx.MCU_Type==0x02)||(DeviceConfig_xx.MCU_ID==0x9033)||(DeviceConfig_xx.MCU_ID==0x7712))
      */
      if ((DeviceConfig_xx.MCU_ID==0x3394)||(DeviceConfig_xx.MCU_ID==0x3378)||(DeviceConfig_xx.MCU_ID==0X3111)||(DeviceConfig_xx.MCU_ID==0X9029)
        ||(DeviceConfig_xx.MCU_ID==0x3401)||(DeviceConfig_xx.MCU_ID==0x3316)||(DeviceConfig_xx.MCU_ID==0x5312)||(DeviceConfig_xx.MCU_ID==0x7022)
        ||((DeviceConfig_xx.MCU_Type==0x02)&&(DeviceConfig_xx.MCU_ID!=0x8132))||(DeviceConfig_xx.MCU_ID==0x9033)||(DeviceConfig_xx.MCU_ID==0x7712)
          ||(DeviceConfig_xx.MCU_ID==0x7311))      
      {
        MC321_MODEL_IN(vdd30);
      }
      else if(DeviceConfig_xx.MCU_ID==0x6060)
      {
        MC30P6060_MODEL_IN(vdd30);
      }
      else if(DeviceConfig_xx.MCU_ID==0x8132)
      {
        WriteFlag=1;//读取用在线进模式时序
        MC32T8132_MODEL_IN(vdd50);
      }
      else if(DeviceConfig_xx.MCU_ID==0x7011)
      {
        MC32P7011_MODEL_IN(vdd30);
      }      
      else if(DeviceConfig_xx.MCU_ID==0x5222)
      {
        MC32P5222_MODEL_IN(vdd30);
      }
      else
      {
        MC321_MODEL_IN(vdd65);
      }
      CLN_VDD2GND_Off;
      VPP12V_On;;
      Delay_1ms(10);
      m301data=MC321_ReadByte(0xffff); //read id
      if(m301data==DeviceConfig_xx.MCU_ID)
      { 
        reFlag=1;
      }
      else
      {reFlag=0;}
    }
    else
    {
      reFlag=OTP_Mode_IN(DeviceConfig_xx.MCU_ID,vpp12,vdd65);
    }
    if(reFlag==0)	
    {
      for(temp=0;temp<5;temp++)
      {
        USB_Tx_Buffer[temp]=Reset[temp];
      }
      ModelIn_result=0;
    }
    else
    {
      for(temp=0;temp<5;temp++)
      {
        USB_Tx_Buffer[temp]=ModelIn[temp];
      }
      ModelIn_result=1;
    }
    USB_SendData(5);
  }
  else if (USB_Rx_Buffer[2]==0x38)  //read OTP; 
  {
    Read_otp_addr=USB_Rx_Buffer[3]*256 +USB_Rx_Buffer[4];
    if(Read_otp_addr==0xc000)
    {
      USB_Rx_Buffer[4]=Read_otp_addr;
    }
    if ((DeviceConfig_xx.ProgramID==0x02)||(DeviceConfig_xx.ProgramID==0x04))
    {
      /*
      if (Read_otp_addr!=0xffff)
      {
        Read_otp_addr=Read_otp_addr/2;
      }
      else
      {
        Read_otp_addr=0x8000;
      }   
      */
      Read_otp_addr=Read_otp_addr;
    }		
    checksum=0x39+33;
    for(temp=0;temp<32;temp++)
    {
      if (DeviceConfig_xx.ProgramID==0x03)
      {
        if (Read_otp_addr>=0x4000)
        {
          OP_STATE=0;
          USB_Tx_Buffer[3+temp]=OTP8K_ReadByte(0x0000 + temp);
        }
        else
        {
          OP_STATE=1;
          USB_Tx_Buffer[3+temp]=OTP8K_ReadByte(Read_otp_addr + temp);
        }				
      }
      else if (DeviceConfig_xx.ProgramID==0x02)
      {
        m301data=MC301_ReadByte(Read_otp_addr);
        Read_otp_addr=Read_otp_addr+1;
        if (m301data==0x3fff)
        {
          m301data=0xffff;
        }
        USB_Tx_Buffer[3+temp]=m301data; //L data
        checksum = checksum +USB_Tx_Buffer[3+temp];
        temp=temp+1;
        USB_Tx_Buffer[3+temp]=m301data>>8;
      }
      else if (DeviceConfig_xx.ProgramID==0x04)
      {
        if(DeviceConfig_xx.MCU_ID==0x5222&&Read_otp_addr>=0xc000)
        {
          m301data=MC32P5222_MTPByteRead(Read_otp_addr);
        }
        else
        {
          m301data=MC321_ReadByte(Read_otp_addr);
        }
        Read_otp_addr=Read_otp_addr+1;
        USB_Tx_Buffer[3+temp]=m301data; //L data
        checksum = checksum +USB_Tx_Buffer[3+temp];
        temp=temp+1;
        USB_Tx_Buffer[3+temp]=m301data>>8;
      }
      else if(DeviceConfig_xx.ProgramID==0x05)
      {
        m301data=MC321_ReadByte(Read_otp_addr);
        Read_otp_addr=Read_otp_addr+1;
        USB_Tx_Buffer[3+temp]=m301data; //L data
        checksum = checksum +USB_Tx_Buffer[3+temp];
        //temp=temp+1;
        //USB_Tx_Buffer[3+temp]=m301data>>8;                          
      }      
      else
      {
        USB_Tx_Buffer[3+temp]=OTP_ReadByte(Read_otp_addr + temp);
      }			
      checksum = checksum +USB_Tx_Buffer[3+temp];
    }
    USB_Tx_Buffer[0]=0x68;
    USB_Tx_Buffer[1]=33;
    USB_Tx_Buffer[2]=0x39;
    USB_Tx_Buffer[35]=checksum;
    USB_Tx_Buffer[36]=0x16;
    USB_SendData(37);
  }
  else if (USB_Rx_Buffer[2]==0x40) //PowerOff
  {
    POWER_OFF(vpp00,vdd00);
    for(temp=0;temp<5;temp++)
    {
      USB_Tx_Buffer[temp]=Reset[temp];
    }
    USB_SendData(5);
    LEDNG_Off;
    LEDBUSY_Off;
    LEDOK_On;
    if (ModelIn_result==1)
    {
      Freq(1);
    }
    else
    {
      Freq(0);
    }
  }
  else if (USB_Rx_Buffer[2]==0xe1)
  {
    for(temp=0;temp<5;temp++)
    {
      USB_Tx_Buffer[temp]=Reset[temp];
    }
    USB_SendData(5);
  }
  USB_Rx_Flag=0x00;
  //#ifndef STM32F10X_CL
  /* Enable the receive of data on EP3 */
     SetEPRxValid(ENDP3);
  //#endif /* STM32F10X_CL */
}

//-----------------------------------
void USB_SendData(u8 Length)
{
  u8 index=0;
  while (Length >= 64)  //一次最多发送64字节
  {
    while(GetEPTxStatus(ENDP1) != EP_TX_NAK);  
    UserToPMABufferCopy(&USB_Tx_Buffer[index], ENDP1_TXADDR, 64);
    SetEPTxCount(ENDP1, 64);
    SetEPTxValid(ENDP1);
    index  += 64;  //计算偏移和剩余字节数
    Length -= 64;
  }
  //发送最后<=64字节的数据
  while(GetEPTxStatus(ENDP1) != EP_TX_NAK); 
  UserToPMABufferCopy(&USB_Tx_Buffer[index], ENDP1_TXADDR, Length);
  SetEPTxCount(ENDP1, Length);
  SetEPTxValid(ENDP1);
  ////clear se_flag
  //USB_Rx_Flag=0x00;
  //remain=63;
  //index=0;
}