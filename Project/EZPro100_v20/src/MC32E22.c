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
#include "MC32E22.h"
#include "MC30P6060.h"
/*******************************************************************************
* Function Name  : MC32E22_W
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void MC32E22_W(u16 OTPData)
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
  
 
//  return;
  
  for(i=0;i<16;i++)
  {         
    if(i==1 || i == 2)
    {
      Delay_10us(3);//30us
    }
    else if(i>=4 && i<=12)
    {
      Delay_100us(2);//200us
    }
    else
    {
      NOP;
    }    
    OTP_SCK_1;  //1
 
    if(i==1 || i == 2)
    {
      Delay_10us(3);//30us
    }  
    else if(i>=4 && i<=12)
    {
      Delay_100us(2);//200us
    }    
    else
    {
      NOP;
    }    
    OTP_SCK_0;
  }  
}

void MC32E22_WriteByte(u16 OTPAddr,u16 OTPData)
{
	u8 i,j,temp;
	u16 a;
        
        for(j=0;j<32;j++)
        {
	MC301_Instruct(SetAddr);
	NOP;
	NOP;
	for(i=0;i<16;i++)
	{
		a =(OTPAddr+j)>>i;
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

	MC32E22_W(OTPData);
        }
        
          //program time
  MC301_Instruct(ProStr);
  
  for(i=0;i<16;i++)
  {       
    OTP_SCK_1;  //1
    Delay_100us(1);
    if (i>3) Delay_100us(2);
    OTP_SCK_0; 
    Delay_100us(1);
    if (i>3) Delay_100us(2);
  }
}


void MC32E22_MODEL_IN(u8 VddFlag)
{
  u8 temp;
  //power off then power on & model in
  VDD65V_Off;//需要的是数字电位器调节出来的5V,把U10出来的6.5V关掉  
  POWER_OFF(vpp00,vdd00);
  //特别注意 Delay_1ms到底延时多久要根据STM32芯片的时钟周期来计算
  Delay_1ms(100);//断电重启 间隔时间稍长一点
  OTP_SCK_0;
  OTP_SDI_0; 

  //给芯片供电
  if (VddFlag == vdd50)//
  {
    VDD_On;
    IO5V_On;//查看开发手册可知6060工作电压可以为5V
  }
  CLN_VDD2GND_Off;//关断Q5后，如果Q14导通则D8导通，VPP差不多等于VDD,9033进入模式的时序需要这样，6060进模式不需要VPP时序,不用VPP时，关断Q14
  Delay_100us(5);
  Delay_1ms(1);
  NOP;

  OTP_SCK_1;//SDI送1
  OTP_SDI_1;
  NOP;
  OTP_SCK_0;
  NOP;
  
  OTP_SCK_1;//SDI送0
  OTP_SDI_0;
  NOP;
  OTP_SCK_0;
  NOP;

  OTP_SCK_1;//SDI送1
  OTP_SDI_1;
  NOP;
  OTP_SCK_0;
  NOP;
  
  OTP_SCK_1;//SDI送1
//  OTP_SDI_1;
  NOP;
  OTP_SCK_0;
  NOP;
  
  OTP_SCK_1;//SDI送0
  OTP_SDI_0;
  NOP;
  OTP_SCK_0;
  NOP;
  
  OTP_SCK_1;//SDI送1
  OTP_SDI_1;
  NOP;
  OTP_SCK_0;
  NOP;
  
  OTP_SCK_1;//SDI送1
//  OTP_SDI_1;
  NOP;
  OTP_SCK_0;
  NOP;
  
  OTP_SCK_1;//SDI送1
//  OTP_SDI_1;
  NOP;
  OTP_SCK_0;
  NOP;
  
  NOP;
  OTP_SCK_1;
  OTP_SDI_0;
  NOP;
  OTP_SCK_0;
  NOP;
  OTP_SCK_1;  
  OTP_SDI_1;
  NOP;
  OTP_SCK_0;
  NOP;
  OTP_SCK_1;
  NOP;
  OTP_SCK_0;
  NOP;
  OTP_SCK_1;
  NOP;
  OTP_SCK_0;
  NOP;
  OTP_SCK_1;
  OTP_SDI_0;
  NOP;
  OTP_SCK_0;
  NOP;
  OTP_SCK_1;
  OTP_SDI_1;
  NOP;
//  Delay_100us(5);
  OTP_SCK_0;
  NOP;
//  Delay_100us(5);
  OTP_SCK_1;
  OTP_SDI_0;
  NOP;
//  Delay_100us(5);
  OTP_SCK_0;
  NOP;
//  Delay_100us(5);
  OTP_SCK_1;
  OTP_SDI_1;
  NOP;
// Delay_100us(5);
  OTP_SCK_0;
  NOP;
// Delay_100us(5);
  OTP_SDI_0;
// Delay_100us(5);
  NOP;
  NOP;
  Delay_1ms(1);
  CLN_VDD2GND_Off;
}
/*******************************************************************************
* Function Name  : MC32T8132_ERASE
* Description    : 全擦或选择某一块进行擦除
* Input          : OTPData 0x20:全擦为0；0x30全擦为1；0x20+num擦除第num块为0；0x30+num擦除第num块为1
* Output         : None
* Return         : Status
*******************************************************************************/
u8 MC32E22_ERASE(u16 OTPData)
{
  u8 i;
  u16 wData,temp=0; 
  u8 wait_time=0;
  u16 ReCnt;
  ReCnt=0;
  u16 Fdata;
  while(++ReCnt<12) // 
  {	
    MC301_Instruct(Erase);
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
    while((temp&0x0001) != 0x0001 )//等待擦除完成 擦除成功 擦除未超时
    {
      Delay_1ms(100);
      temp = MC321_ReadByte(0XFFFE);
      
      wait_time++;
      if(wait_time>60)//
      {
        wait_time = 0;        
        break;//擦除失败
      }
    }
    if(temp == 0x0009)
    {
      return 1;//擦除成功 
    }
    else
    {
      //power off then power on & model in
      MC32E22_MODEL_IN(vdd50);
      Fdata=MC321_ReadByte(0XFFFF);
      if (Fdata !=DeviceConfig_xx.MCU_ID) 
      {
        ERORR_VALUE=OTP_ModeIn_false;
        return 0;
      }         
    }
    if (ReCnt==10)//4
    {
      temp = temp & 0x0007;
      if(!(temp & 0x0001))
      {
        temp = 17;
      }
      else if(temp == 0x0003)
      {
        temp = 18;
      }
      else if(temp == 0x0005)
      {
        temp = 19;
      }
      else if(temp == 0x0007)
      {
        temp = 20;
      }
      return temp;
    }	     
  }
  return 0;//擦除成功
}
void MC32E22FTtest(u8 IRC_Option)
{
  u16 i;
  u16 cnt[3];

  MC321_IRC_INST(0x3c00+IRC_Option);
  MC321_IRC_INST(0x57fa);//功能测试模式下，频率校准位对应的寄存器地址0x1fa
  
//  MC321_IRC_INST(0x3c01);//设置测试模式为 高频系统时钟由PSDO输出     
//  MC321_IRC_INST(0x57f7);  
  
  MC321_IRC_INST(0x0000);//开发手册上指出：每条指令的运行是在下一条指令时钟输入后运行
//  MC321_IRC_INST(0x0000);
//  MC321_IRC_INST(0x0000);
  Delay_1ms(1);//需要的延时时间按芯片实际测试效果决定 要求是等波形稳定后测频率 
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
//--------------------------------------------------------
u8 MC32E22IRC(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i,j,optfa;
  u16 temp0_FT,temp1_FT;
  u8 mm=0x80;
  u16 Fdata;
  
  //power off then power on & model in
  MC32E22_MODEL_IN(vdd50);
//  Delay_1ms(1);
//  CLN_VDD2GND_Off;
//  VPP_On;
//  Delay_1ms(1);
  
  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID) 
  {
    ERORR_VALUE=IRC_ModeIn_false;
    return 0;
  }
//  Delay_1ms(1000);
  MC301_Instruct(TestMod);    //进入测试模式    
  for(j=0;j<5;j++)
  {
      MC321_IRC_INST(0X0000); //5 NOP
  }
  
  //先从flash中读PC发OPTION2的HIRC电压校准位(BIT15)和HIRC温度校准位(BIT14:9) 
  if (FMReadOne(Addr_Flash_OptionProtec+2*2+1)==1)
  {
    optfa = Rxdata;//
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    return 0;
  }    
  optfa = optfa & 0xfe;//   
  
  //以下设置OPTION寄存器 
  //OPTION0
  MC321_IRC_INST(0x3c00+0x80);        //movai 0x80    //不使能代码加密位
  MC321_IRC_INST(0x57f9);             //movra 0x1f9
  MC321_IRC_INST(0x3c00+0x70);        //movai	0x60    //128分频
  MC321_IRC_INST(0x57f8);             //movra	0x1f8  
  //OPTION1
  MC321_IRC_INST(0x3c00+0x00);        //movai 0x00 
  MC321_IRC_INST(0x57fb);             //movra 0x1fb
  MC321_IRC_INST(0x3c00+0x00);        //movai 0x00
  MC321_IRC_INST(0x57fa);             //movra	0x1fa
  //OPTION2
  //HIRC电压校准位和HIRC温度校准位
  MC321_IRC_INST(0x3c00+0x00+optfa);  //movai   optfa 
  MC321_IRC_INST(0x57fd);             //movra	0x1fd
  MC321_IRC_INST(0x3c00+0x00);        //movai	0x00
  MC321_IRC_INST(0x57fc);             //movra	0x1fc
  //OPTION3
  MC321_IRC_INST(0x3c00+0x00);        //movai 0x00 
  MC321_IRC_INST(0x57ff);             //movra 0x1ff
  MC321_IRC_INST(0x3c00+0x00);        //movai 0x00
  MC321_IRC_INST(0x57fe);             //movra	0x1fe
  
//  mm=0x0080; //8位频率校准位256 中间值128
  MC321_IRC_INST(0x3c01);//设置测试模式为 高频系统时钟由PSDO输出     
  MC321_IRC_INST(0x57f7); 
  MC321_IRC_INST(0X0000);
  OPTION_FRT=0; //initial 
  for(i=0;i<8;i++)
  {
    MC32E22FTtest(mm);
    //以下判断频率是否满足要求
    if (IRC_VALUE == IRC_FreqType)
    {
      break;
    }
    //以下不满足做处理 并继续校准
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
  MC32E22FTtest(mm+1);
  temp0_FT=IRC_VALUE;

  MC32E22FTtest(mm);
  temp1_FT=IRC_VALUE;

  if((temp0_FT-IRC_FreqType)<(IRC_FreqType-temp1_FT))
  {
    mm=mm+1;
    IRC_VALUE=temp0_FT;
  }       
  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    OPTION_FRT=mm;
    return 1;//校准成功 
  } 
  OPTION_FRT=0XFF;
  return 0;//校准失败 
}
/*******************************************************************************
* Function Name  : MC32E22_Program
* Description    : MC32E22有擦除功能，先判断本次操作是不是擦除，如果是擦除，就
*                  只进行擦除操作；是烧写程序，就不进行擦除操作，只烧写
* Input          : None
* Output         : None
* Return         : 1 :成功  0：失败
*******************************************************************************/
u8 MC32E22_Program()
{
  u8    ReCnt=0;
  u16   Fdata,ROMReadData;
  u8	FdataL,FdataH;  
  u8    ReFlag=0;
  u16   CheckSum=0;
  u16   mcu_addr;   
  u32   op_addr;

 //-------------------------以下擦除相关代码-----------------------
/*
  //先判断本次操作是只擦除  还是只编程  还是擦除后再编程
  //此处和上位机通讯规定 flash的Addr_Flash_OptionProtec+5地址存要擦除的块（两个字节）；flash的Addr_Flash_OptionProtec+7地址存 擦除后的值（0或1）
  FM_CS_0;
  FMReadMore(Addr_Flash_OptionProtec+9);//通过读前一个地址 读完地址加1 设置下次要读的地址Addr_Flash_OptionProtec+5*2-1
//  FMReadMore(Addr_Flash_OptionProtec+1);
  Fdata =Rxdata;
  SPI_ReadByte();//从flash中取一个字节
  FdataL=Rxdata;
  CheckSum +=FdataL;
  SPI_ReadByte();//再从flash中取一个字节
  FdataH=Rxdata;
  CheckSum +=FdataH;
  Fdata=FdataH*0x100+FdataL;//组成一个字  
  erase_num = Fdata;
  SPI_ReadByte();//从flash中取一个字节
  FdataL=Rxdata;
  CheckSum +=FdataL;
  SPI_ReadByte();//再从flash中取一个字节
  FdataH=Rxdata;
  CheckSum +=FdataH;
  Fdata=FdataH*0x100+FdataL;//组成一个字  
  erase_en = Fdata;
  FM_CS_1;
  
  //power off then power on & model in
  MC32E22_MODEL_IN(vdd50);
  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID) 
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  }
  temp = erase_en & 0x0006;
  if(temp == 0x00 || temp == 0x06)//需要擦除
  {//和PC通讯协议是 ：00-擦除并编程；01-只编程：11-只擦除
    temp = (((u8)erase_en) & 0x01)<<4;//擦为1或0
    if(erase_num == 0x00)//全擦 即擦除未锁住的块
    {
      ReFlag = MC32E22_ERASE(0x20+temp);//全擦为1或0
      if(ReFlag != 1)//擦除失败
      {
        ERORR_VALUE = ReFlag;//
        return 0;
      }
    }
    else//块擦
    {
      for(i=0;i<16;i++)
      {
        temp1 = erase_num&(0x0001<<i);
        if(temp1)//需要擦除第i块
        {
          ReFlag = MC32E22_ERASE(0x00+temp+i);//块擦为1或0
          if(ReFlag == 1)
          {
            //power off then power on & model in
            MC32E22_MODEL_IN(vdd50);  
            Fdata=MC321_ReadByte(0XFFFF);
            if (Fdata !=DeviceConfig_xx.MCU_ID) 
            {
              ERORR_VALUE=OTP_ModeIn_false;
              return 0;
            }            
          }
          else//擦除失败
          {
            ERORR_VALUE = ReFlag;//
            return 0;
          }
        }
      }
    }
    if((erase_en & 0x0006) == 0x06)//只擦除
    {
      return 1;
    }
  }
  */
//-------------------------以下校验相关代码----------------------- 
//  if (DeviceConfig_xx.IRC_Select==0x01)//正式代码
  if (DeviceConfig_xx.IRC_Select==0x00)//调试代码
  {
    ReFlag=MC32E22IRC(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);   
    if(ReFlag == 0) 
    {
      ERORR_VALUE=IRC_Value_false;
      return 0;
    }
  }
//-------------------------以下写ROM相关代码----------------------- 
  //power off then power on & model in
  MC32E22_MODEL_IN(vdd50);
  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID) 
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  }
//Write ROM
  if (DeviceConfig_xx.WriteEn & 0x0f)//
//  if(DeviceConfig_xx.WriteEn == 0)//测试
  {
    CheckSum=0;
    FM_CS_0;
    FMReadMore(Addr_Flash_ROMStart+DeviceConfig_xx.RomFirAddr-1);
    Fdata =Rxdata;
    
    ROMReadData=MC321_ReadByte(0x0000); //set ADDR
    for(OTP_ADDR=DeviceConfig_xx.RomFirAddr;OTP_ADDR<=DeviceConfig_xx.RomEndAddr;)
    {
      SPI_ReadByte();//从flash中取一个字节
      FdataL=Rxdata;
      CheckSum +=FdataL;
      SPI_ReadByte();//再从flash中取一个字节
      FdataH=Rxdata;
      CheckSum +=FdataH;
      Fdata=FdataH*0x100+FdataL;//组成一个字

      mcu_addr=OTP_ADDR/2;
      OTP_ADDR=OTP_ADDR+2;//已经读了两个字节数据 
      
      Fdata = 0x55aa;
      
      ReCnt=0;
      while(++ReCnt<6)  
      {	
        if (Fdata == 0xffff)  // 
        {
          ROMReadData=MC321_R();
          if(ROMReadData == 0xffff)
          {//不需要写
            break;
          }
        }
        ROMReadData=MC321_ReadByte(0x0000);
        ROMReadData=MC321_ReadByte(0x8000);
        ROMReadData=MC321_ReadByte(0xc000);
        ROMReadData=MC321_ReadByte(0xf000);
        MC32E22_WriteByte(0x8000,0xaa55);
        ROMReadData=MC321_ReadByte(0x8000);
        MC32E22_W(Fdata);
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
      //以下判断是否写完
      if (OTP_ADDR<DeviceConfig_xx.RomEndAddr)
      {//没有写完 rom地址加1 继续写
        MC301_Instruct(AddrINC);
      }
    }
    FM_CS_1;
  }
 //-------------------------以下写OPTION相关代码----------------------- 
  //Write OPTION  
  if(DeviceConfig_xx.WriteEn & 0xf0)
  {
    FM_CS_0;
    FMReadMore(Addr_Flash_OptionProtec-1);
    Fdata =Rxdata;
    
    ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr); //set ADDR
    for(op_addr=0;op_addr<DeviceConfig_xx.OptionSize;op_addr++)//
    {
      SPI_ReadByte();//从flash中取一个字节
      FdataL=Rxdata;
      CheckSum +=FdataL;
      SPI_ReadByte();//再从flash中取一个字节
      FdataH=Rxdata;
      CheckSum +=FdataH;
      Fdata=FdataH*0x100+FdataL;//组成一个字

      //option0 //
      /*
      if(op_addr == 0)
      {
        Fdata = 0xfcf0;//用内部高频
      }*/
      //option1 
      if(op_addr==1)
      {
        Fdata = (Fdata&0xff00) + OPTION_FRT;       //校准得到的频率校准值
      }
      //option2                 //根据测试结果 正式产品需要写死                                                
      if(op_addr==2)//电压选择位 温度校准位 测试阶段 根据上位机写  
      {
      //  Fdata=0xBF;//如果HIRC电压选择位选1.9V，则是0xBF
      //  Fdata=0xAF;//如果HIRC电压选择位选1.7V，则是0xAF        
      }
      //option3 //不需要改
      ReCnt=0;
      while(++ReCnt<6)  
      {	
        if (Fdata == 0xffff)  // 
        {
          ROMReadData=MC321_R();
          if(ROMReadData == 0xffff)
          {//不需要写
            break;
          }
        }
        MC321_W(Fdata);
        ROMReadData=MC321_R();
        if (ROMReadData==Fdata)
        {
          ReCnt=0;
          break;
        }
        else if (ReCnt==4)
        {
          ERORR_VALUE=OPTION_Write_false;
          OTP_ADDR= mcu_addr;
          return 0;
        }	     
      }
      //以下判断是否写完
      if (op_addr<DeviceConfig_xx.OptionSize-1)
      {//没有写完 rom地址加1 继续写
        MC301_Instruct(AddrINC);
      }
    }
    FM_CS_1;
  }
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  return 1;
}