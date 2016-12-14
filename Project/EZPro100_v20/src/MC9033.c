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

/*******************************************************************************
* Function Name  : NomallCheck
* Description    : 
* Input          : 
* Output         : 
*******************************************************************************/
u8 NomallCheck(u16 Fdata,u16 mcu_addr)
{
  u8   i;
  u16   ROMReadData;
  u8    SuccessCnt;     
  SuccessCnt = 0;
  if(WriteConfig_xx.WriteCheckNeed)
  {
    for(i=0;i<WriteConfig_xx.ReadCnt;i++)
    {
      ROMReadData=MC321_R();
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
    //break;
    return 1;
  }
  else
  {
    ERORR_VALUE=ROM_Write_false;
  }        
  return 0;
}
/*******************************************************************************
* Function Name  : CheckRom
* Description    : 
* Input          : 
* Output         : 
*******************************************************************************/
u8 CheckRom(u16 data,u16 mcu_addr)
{
  u8  i;
  u16  Fdata,ROMReadData;
  u8    j;
  u8    SuccessCnt;   
   
  if (WriteConfig_xx.AllCheckNeed == 2)
  {
    for(i=1;i<3;i++)
    {
      SuccessCnt = 0;
      for(j=0;j<WriteConfig_xx.ReadCnt;j++)
      {
        ROMReadData=MC321_R_DiffMode(i);
        if (ROMReadData==data)
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
        if(i==1)
        {
          ERORR_VALUE=MAGRIN1_ERROR;
        }
        else if(i==2)//测试用
        {
          ERORR_VALUE=OFF_MAGRIN_ERROR;
        }
        break;
      }
    }  
    if (i==3)//3种模式读取都和写入的数据一致 烧写成功 2
    {
      //break;
      return 1;
    }
    else
    {
      OTP_ADDR= mcu_addr;
      return 0;
    }
  }
  else if(WriteConfig_xx.AllCheckNeed == 1)
  {
    SuccessCnt = 0;
    for(j=0;j<WriteConfig_xx.ReadCnt;j++)
    {
      ROMReadData=MC321_R();
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
      //break;
      return 1;
    }
    else
    {
      ERORR_VALUE=NOMALL_ERROR;
      OTP_ADDR= mcu_addr;
      return 0;
    }
  }
  else if(WriteConfig_xx.AllCheckNeed == 3)
  {
    for(i=0;i<2;i++)
    {
      SuccessCnt = 0;
      for(j=0;j<WriteConfig_xx.ReadCnt;j++)
      {
        ROMReadData=MC9033_ReadByte_DiffMode(i,mcu_addr*2);
        if (ROMReadData==data)
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
        if(i==1)
        {
          ERORR_VALUE=MAGRIN1_ERROR;
        }
        else if(i==2)//测试用
        {
          ERORR_VALUE=OFF_MAGRIN_ERROR;
        }
        break;
      }
    }  
    if (i==2)//2种模式读取都和写入的数据一致 烧写成功 2
    {
      //break;
      return 1;
    }
    else
    {
      OTP_ADDR= mcu_addr;
      return 0;
    }
    
  }
  return 0;
}
/*******************************************************************************
* Function Name  : MC9033_SetRmode.
* Description    : 设置不同读取模式
* Input          : ModeNum 0：普通模式；1：Margin-1模式；
* Output         : none
*******************************************************************************/
void MC9033_SetRmode(u8 ReadMode)
{
  u8 i,temp; //temp use for Nop instruce
  u16 a,wData;
  if(ReadMode == 0)//普通读取
  {
    a = 0x0000;
  }
  else if(ReadMode == 1)//Margin-1模式读取
  {
    a = 0x0002;
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

/*******************************************************************************
* Function Name  : MC9033_ReadByte_DiffMode
* Description    : MC9033不同模式读数据（设置读取地址）
* Input          : ModeNum 0：普通模式；1：Margin-1模式；2：Margin-2模式
*                  OTPAddr：读取地址
* Output         : a读取到的数据
*******************************************************************************/

u16 MC9033_ReadByte_DiffMode(u8 ModeNum,u16 OTPAddr)
{
  u8 i,temp;
  u16 a;

  //set read mode 
  MC9033_SetRmode(ModeNum);
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
/*******************************************************************************
* Function Name  : MC9033_R_DiffMode
* Description    : 不同模式读数据（不需要设置读取地址，之前设置过初始地址，读过数据，并且发了地址加1命令）
* Input          : ModeNum 0：普通模式；1：Margin-1模式；
* Output         : read_data:读取到的数据
*******************************************************************************/
u16 MC9033_R_DiffMode(u8 ModeNum)
{
  u8 i,temp; //temp use for Nop instruce
  u16 a,read_data;
  //set read mode
  MC9033_SetRmode(ModeNum);

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
void MC9033_IRC_INST(u16 OTPData)
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
  NOP;
  NOP;
  NOP;
  NOP;
  
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
void MC9033FTtest(u16 irc_adj_value,u8 tempadj)
{
  u16 temp=0xf000;
  u8  adjH,adjL,optf1;
  u16 i;
  u16 cnt[3];

  adjH=irc_adj_value/256; //H
  adjL=irc_adj_value%256; //OPTBIT1 该配置的值  （HIRC频率校准位低八位）

  temp=0xf100+adjL;//高8位为寄存器地址 0xf1，低8位为数据adjL
  // temp=0x3c00+i;
  MC321_IRC_INST(temp);//配置OPTBIT1寄存器
  //MC321_IRC_INST(0x57fb);

  optf1=adjH | tempadj;

  temp=0xf200+optf1;
  MC321_IRC_INST(temp);//配置OPTBIT2寄存器

  MC321_IRC_INST(0xff01) ; //HIRC ouput mode 设置测试功能寄存器（地址0xFF）值为0x01  即高频系统时钟输出

  MC321_IRC_INST(0x0000);//空指令
  MC321_IRC_INST(0x0000);
     
  //----------------------------
  //用STM32的TIMER来计算外部脉冲频率  具体怎么实现 暂时还没看懂 需要问莫工
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
  IRC_VALUE= (u16)((cnt[0]+cnt[1]+cnt[2])/3 );//检测得到的频率值
 }

void MC9033_Instruct(u16 OTPData)
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
void MC9033_MODEL_IN(u8 VddFlag)
{
  u8 temp;
  //power on & model in
  POWER_OFF(vpp00,vdd00);
  //Delay_1ms(100);
  Delay_1ms(5);
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
//  VDD_IO_Off; //VDD IO PIN SIGNAL

  NOP;
  NOP;
  NOP
//  VDD_IO_On;

  CLN_VDD2GND_Off;
  VPP12V_On;
//  VPP_IO_On;
  Delay_100us(5);
  Delay_1ms(1);
  //Delay_100us(2);
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
  OTP_SDI_1;
  NOP;
  OTP_SDI_0;
  NOP;

  VPP12V_Off;
//  VPP_IO_Off;
  CLN_VDD2GND_On;
  Delay_100us(5);
  Delay_1ms(1);
  NOP;

  OTP_SDI_1;
  NOP;
  OTP_SDI_0;
  NOP;

  CLN_VDD2GND_Off;
  VPP12V_On;
//  VPP_IO_On;
  Delay_100us(5);
  Delay_1ms(1);
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
//  VPP_IO_Off;
  CLN_VDD2GND_On;
  Delay_100us(5);
  Delay_1ms(1);
  NOP;

  OTP_SDI_1;
  NOP;
  OTP_SDI_0;
  Delay_1ms(1);
  NOP;
}
//--------------------------------------------------------

u8 M9033IRC(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i,j,k, optfa;
  u16 mm,temp0_FT,temp1_FT;
  u8 tempadj;

//  mm=0x0400; // & (rdv<<8);
  //power on & model in 

//温度值0x12
  for(k=0;k<7;k++)//0x12 0x11 0x13 0x10 0x14 0x09 0x15 7个温度校准值
  {
    mm = 0x400;
    for(i=0;i<11;i++)
    {                 
      MC9033_MODEL_IN(vdd30);
      CLN_VDD2GND_Off;
      VPP12V_On;
//      VPP_IO_On;

      Delay_1ms(1);
      
      //以下进入测试模式  之后才能设置OPTION寄存器
      MC9033_Instruct(TestMod);       
      for(j=0;j<5;j++)
      {
          MC9033_IRC_INST(0X0000); //5 NOP
      }

      //先从flash中读出
      //特别说明：为了和16位串行数据格式的IC兼容，PC发送数据给烧写器时，已经把8位数据扩展成16位，如0xC5，发送时发送0xC5 0x00
      //所以存入flash的数据也是扩展后的16位的数据
      if (FMReadOne(0x0100E4)==1)//从flash固定地址(0x010000+0x72*2)取OPBIT2值（bit[7:3]是温度校准位）
        //（该值在测试阶段是PC端发给烧写器的，烧写器存在固定地址，但是正式产品的温度校准值是经过测试后确定的某一个值）
      {
        optfa =Rxdata;
      }
      else
      {
        ERORR_VALUE=FM_Read_false;
        return 0;
      } 
      if(k==0)
      {
        optfa = (optfa & 0x07) | (0x12<<3);        
      }
      else if(k==1)
      {
        optfa = (optfa & 0x07) | (0x11<<3);         
      }
      else if(k==2)
      {
        optfa = (optfa & 0x07) | (0x13<<3);         
      }   
      else if(k==3)
      {
        optfa = (optfa & 0x07) | (0x10<<3);         
      }      
      else if(k==4)
      {
        optfa = (optfa & 0x07) | (0x14<<3);         
      }      
      else if(k==5)
      {
        optfa = (optfa & 0x07) | (0x09<<3);         
      }      
      else if(k==6)
      {
        optfa = (optfa & 0x07) | (0x15<<3);         
      }      
      
//      optfa = (optfa & 0x07) | (0x12<<3);
      //以下设置OPTION寄存器 （除HIRC频率校准位和HIRC温度校准位）  具体值根据设计人员要求配置
      MC9033_IRC_INST(0xf000);
      MC9033_IRC_INST(0xf100);
    //  MC9033_IRC_INST(0xf3BF);//如果HIRC电压选择位选1.4V，则是0xf3BF
      MC9033_IRC_INST(0xf3AF);//如果HIRC电压选择位选1.55V，则是0xf3AF
      temp0_FT=0xf200+optfa;
      MC9033_IRC_INST(temp0_FT);
      
      tempadj=optfa & 0xf8;//取OPTION2中的BIT[7:3]即HIRC温度校准位

      OPTION_FRT=0; //initial
      OPTION_FRT2=0;
      MC9033FTtest(mm,tempadj);//设置HIRC频率校准位(mm)和HIRC温度校准位(tempadj)  并检测芯片的SDO输出脉冲频率

      if (IRC_VALUE == IRC_FreqType)
      {//得到需要的频率 校准完成
              break;
      }
      if (IRC_VALUE<IRC_FreqType)
      {//得到的频率小于需要的 温度校准值除2 再设置OPTION并检测 
              mm |=0x400 >>i;
      } 
      else
      {//
              mm &=~(0x400>>i);
      }
      mm |=0x200>>i;
    }

    MC9033FTtest(mm+1,tempadj);
    temp0_FT=IRC_VALUE;

    MC9033FTtest(mm,tempadj);
    temp1_FT=IRC_VALUE;

    if((temp0_FT-IRC_FreqType)<(IRC_FreqType-temp1_FT))
    {
      mm=mm+1;
      IRC_VALUE=temp0_FT;
    }

    if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
    {
      OPTION_FRT=mm/256;
      OPTION_FRT =OPTION_FRT | tempadj;
      OPTION_FRT2=mm%256;
      return 1;
    }    
  }
  OPTION_FRT=0xff;
  OPTION_FRT2=0xff;
  return 0;
}

u8 M9033IRC_Check(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{

  u16 mm;
  u8 tempadj;   
      
  tempadj=OPTION_FRT&0xf8;
  mm=OPTION_FRT&0x07;
  mm=mm*256+OPTION_FRT2;
  MC9033FTtest(mm,tempadj);//设置HIRC频率校准位(mm)和HIRC温度校准位(tempadj)  并检测芯片的SDO输出脉冲频率

  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    //OPTION_FRT=mm/256;
    //OPTION_FRT =OPTION_FRT | tempadj;
    //OPTION_FRT2=mm%256;
    return 1;
  }    
  OPTION_FRT=0xff;
  OPTION_FRT2=0xff;
  return 0;
}

u8 M9033IRC_ModeInCheck(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 j;
  u16 mm,temp0_FT;
  u8 tempadj;
     
  //以下进入测试模式  之后才能设置OPTION寄存器
  MC9033_Instruct(TestMod);       
  for(j=0;j<5;j++)
  {
      MC9033_IRC_INST(0X0000); //5 NOP
  }
      
  //以下设置OPTION寄存器 （除HIRC频率校准位和HIRC温度校准位）  具体值根据设计人员要求配置
  MC9033_IRC_INST(0xf000);
  MC9033_IRC_INST(0xf100);
  MC9033_IRC_INST(0xf3AF);//如果HIRC电压选择位选1.55V，则是0xf3AF
  temp0_FT=0xf200+OPTION_FRT;
  MC9033_IRC_INST(temp0_FT);
      
  tempadj=OPTION_FRT&0xf8;
  mm=OPTION_FRT&0x07;
  mm=mm*256+OPTION_FRT2;
  MC9033FTtest(mm,tempadj);//设置HIRC频率校准位(mm)和HIRC温度校准位(tempadj)  并检测芯片的SDO输出脉冲频率

  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    //OPTION_FRT=mm/256;
    //OPTION_FRT =OPTION_FRT | tempadj;
    //OPTION_FRT2=mm%256;
    return 1;
  }    
  OPTION_FRT=0xff;
  OPTION_FRT2=0xff;
  return 0;
}



//-----------------------------------------------------------------
u8 MC9033_Promgram()
{
  u8    ReCnt=0;
  u16   Fdata,ROMReadData;
  u8    ReFlag=0;
  u16   CheckSum=0;
  u16   mcu_addr=0;
  
  u8    ForceReCnt=0;  

  
  WriteConfig_xx.WriteCheckNeed=0;
  WriteConfig_xx.ReadCnt=1;
  WriteConfig_xx.ReadOkCnt=1;
  WriteConfig_xx.ReWriteCnt=1;
  WriteConfig_xx.ForceWriteCnt=1;
  WriteConfig_xx.AllCheckNeed=3;  
  
//------------------------------先进模式----------------------------
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(5);
  //power on & model in
  MC9033_MODEL_IN(vdd30);
  CLN_VDD2GND_Off;
  VPP12V_On;
  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID) 
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  }
  
//------------------------------IRC校准----------------------------     
  if (DeviceConfig_xx.IRC_Select==0x01)//正式代码
//  if (DeviceConfig_xx.IRC_Select==0x00)//调试代码
  {
    ReFlag=M9033IRC(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType); 
    if(ReFlag==1)
    {
      ReFlag=M9033IRC_Check(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);  
    }
    ConfigTIM4();//关闭Timer3、Timer4
    if(ReFlag == 0) 
    {
      ERORR_VALUE=IRC_Value_false;
      return 0;
    }
  }
  
//------------------------------再进模式----------------------------  
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(5);
  //power on & model in
  MC9033_MODEL_IN(vdd30);
  CLN_VDD2GND_Off;
  VPP12V_On;
  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID) 
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  }
  
//------------------------------Write ROM----------------------------   
  if (DeviceConfig_xx.WriteEn & 0x0f)
  {
    CheckSum=0;
    FM_CS_0;
    FMReadMore(Addr_Flash_ROMStart+DeviceConfig_xx.RomFirAddr-1);
    Fdata =Rxdata;

    ROMReadData=MC321_ReadByte(0x0000); //set ADDR
    for(OTP_ADDR=DeviceConfig_xx.RomFirAddr;OTP_ADDR<=DeviceConfig_xx.RomEndAddr;)
    {
      SPI_ReadByte();
      Fdata=Rxdata;
      SPI_ReadByte();
      CheckSum +=Fdata;
      
      mcu_addr=OTP_ADDR/2;

      //change option value
      if (OTP_ADDR==0x71*2)
      {
        ROMReadData=MC321_ReadByte(OTP_ADDR);
        if(ROMReadData==0xff)
            Fdata=OPTION_FRT2;
        else
            Fdata=ROMReadData;
        WriteConfig_xx.OptionValue[0] = Fdata;//烧option之前先把值存起来，以方便后续全面校验时读值
        WriteConfig_xx.OptionAddr[0]=OTP_ADDR;        
      }
      if (OTP_ADDR==0x72*2)
      {
        ROMReadData=MC321_ReadByte(OTP_ADDR);
        if(ROMReadData==0xff)
            Fdata=OPTION_FRT;
        else
            Fdata=ROMReadData;
        WriteConfig_xx.OptionValue[1] = Fdata;//烧option之前先把值存起来，以方便后续全面校验时读值
        WriteConfig_xx.OptionAddr[1]=OTP_ADDR;        
      }
      if (OTP_ADDR==0x73*2)//HIRC电压选择位
      {
        Fdata=(Fdata & 0xEF);//1.55V
        WriteConfig_xx.OptionValue[2] = Fdata;//烧option之前先把值存起来，以方便后续全面校验时读值
        WriteConfig_xx.OptionAddr[2]=OTP_ADDR;         
      }
      
      
      ReCnt=WriteConfig_xx.ReWriteCnt;
      while(ReCnt--)  
      {	                   
        ForceReCnt=WriteConfig_xx.ForceWriteCnt;
        if (Fdata!=0xffff)
        {
          while(ForceReCnt--)
          {
            MC321_WriteByte(OTP_ADDR,Fdata);//MC9033实现写一个字节数据到OTP_ADDR地址:先送这个数据到OTP_ADDR地址，再送这个数据到OPT_ADDR+1的地址（芯片会自己分辨、处理高四位和低四位）
            MC321_WriteByte(OTP_ADDR+1,Fdata);
          }
        }
        if((OTP_ADDR == 0xee)||(OTP_ADDR == 0xef))
        {  //这两个值不校验
          //mcu_addr=OTP_ADDR/2;
          break;
        }   
        else
        {
          if(NomallCheck(Fdata,mcu_addr)==0)
          {
            
          }
          else
          {
            break;
          }
        }
        /*
        for(i=0;i<2;i++)
        {
          ROMReadData=MC9033_ReadByte_DiffMode(i,OTP_ADDR);
          if((OTP_ADDR == 0xee)||(OTP_ADDR == 0xef))
          {  //这两个值不校验
          }    
          else
          {
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
              
              break;
            }
          }
        } 
        if(i==2)//2种模式读取都和写入的数据一致 烧写成功 1
        {
          ReCnt=0;
          break;
        }
        else if (ReCnt==4)
        {
          if(ERORR_VALUE!=NOMALL_ERROR&&ERORR_VALUE!=MAGRIN1_ERROR&&ERORR_VALUE!=OFF_MAGRIN_ERROR)
          {             
            ERORR_VALUE=ROM_Write_false;
          }
          return 0;
        }                            
       */
        if (ReCnt==0)
        {
          OTP_ADDR= mcu_addr;
          return 0;
        } 
        
      }
      OTP_ADDR=OTP_ADDR+2;
    }
    FM_CS_1;
  }
  
//------------------------------频率验证 取写入芯片的温度校准值和频率校准值---------------------------- 
  if (DeviceConfig_xx.IRC_Select==0x01)//正式代码
  {
    OPTION_FRT2=MC321_ReadByte(0x71*2);
    OPTION_FRT=MC321_ReadByte(0x72*2);
    ReFlag=M9033IRC_ModeInCheck(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType); 
    ConfigTIM4();//关闭Timer3、Timer4
    if(ReFlag == 0) 
    {
      ERORR_VALUE=IRC_Value_false;
      return 0;
    }
  }

//------------------------------再进模式----------------------------  
  POWER_OFF(vpp00,vdd00);
  Delay_1ms(5);
  //power on & model in
  MC9033_MODEL_IN(vdd30);
  CLN_VDD2GND_Off;
  VPP12V_On;
  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID) 
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  }
  
//------------------------------rom 全面读校验----------------------------    
  if (DeviceConfig_xx.WriteEn & 0x0f)
  {
    if (WriteConfig_xx.AllCheckNeed > 0)
    {
      FM_CS_0;
      FMReadMore(Addr_Flash_ROMStart+DeviceConfig_xx.RomFirAddr-1);
      Fdata =Rxdata;
      
      ROMReadData=MC321_ReadByte(0x0000); //set ADDR
      for(OTP_ADDR=DeviceConfig_xx.RomFirAddr;OTP_ADDR<=DeviceConfig_xx.RomEndAddr;)
      {
        SPI_ReadByte();
        Fdata=Rxdata;
        SPI_ReadByte();
	
	mcu_addr=OTP_ADDR/2;
        if (OTP_ADDR==0x71*2)
        {
          Fdata = WriteConfig_xx.OptionValue[0];       
        }
        if (OTP_ADDR==0x72*2)
        {
          Fdata = WriteConfig_xx.OptionValue[1];        
        }
        if (OTP_ADDR==0x73*2)//HIRC电压选择位
        {
          Fdata = WriteConfig_xx.OptionValue[2];        
        }	
        
        if((OTP_ADDR == 0xee)||(OTP_ADDR == 0xef))
        {  //这两个值不校验
          //mcu_addr=OTP_ADDR/2;
          //break;
        }   
        else
        {
          if(CheckRom(Fdata,mcu_addr)==0)
          {
            return 0;
          }
        }
        
        OTP_ADDR=OTP_ADDR+2;
      }
      FM_CS_1;
    }
  }
    
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  return 1;
}











