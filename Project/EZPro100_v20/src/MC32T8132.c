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
#include "MC32T8132.h"
#include "MC9033.h"
/*******************************************************************************
* Function Name  : MC32T8132_MODEL_IN
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void MC32T8132_MODEL_IN(u8 VddFlag)
{
  u8 temp;
  //power off then power on & model in
  VDD65V_Off;//需要的是数字电位器调节出来的5V,把U10出来的6.5V关掉  
  POWER_OFF(vpp00,vdd00);

  if(WriteFlag)//在线烧录
  {
    Delay_1ms(1200);
  }
  else//裸片烧录
  {
    Delay_1ms(50);
  }
  OTP_SCK_0;
  OTP_SDI_0; 
  if (VddFlag==vdd50)
  {
    VDD_On;
    IO5V_On;//
  }
  CLN_VDD2GND_Off;

   if(WriteFlag)//在线烧录
  {
    Delay_100us(300);
  }
  else//裸片烧录
  {
    Delay_100us(3);
  }
  VPP_On; 
  Delay_100us(3);
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

  VPP_Off; 
  CLN_VDD2GND_On;
  Delay_100us(3);

  NOP;

  OTP_SDI_1;
  NOP;
  OTP_SDI_0;
  NOP;

  CLN_VDD2GND_Off;
  VPP_On; 
  Delay_100us(3);
  NOP;

  OTP_SDI_1;
  NOP;
  OTP_SDI_0;
  NOP;
  OTP_SDI_1;
  NOP;
  OTP_SDI_0;
  NOP;
  VPP_Off; 
  CLN_VDD2GND_On;
  Delay_100us(3);
  NOP;

  OTP_SDI_1;
  NOP;
  OTP_SDI_0;
  Delay_1ms(1);
  NOP;
  
  Delay_1ms(1);
  CLN_VDD2GND_Off;
  VPP_On;
//  if(WriteFlag)//在线烧录
//  {
    Delay_1ms(200);
//  }  
}
/*******************************************************************************
* Function Name  : MC32T8132_ERASE
* Description    : 全擦或选择某一块进行擦除
* Input          : OTPData 0x20:全擦为0；0x30全擦为1；0x20+num擦除第num块为0；0x30+num擦除第num块为1
* Output         : None
* Return         : Status
*******************************************************************************/

u8 MC32T8132_ERASE(u16 OTPData)
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
      MC32T8132_MODEL_IN(vdd50);
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
  return 0;//
}
void MC32T8132FTtest_8Bit(u8 OSCCAL,u8 TADJ)
{
  u16 temp;
  u8  adjL;
  u16 i;
  u16 cnt[3];
  
  //OPTION 2：
  //bit14:9 --  bit5:bit0 TADJ  

  //OPTION 1：
  //bit7:0  --  bit7:0   OSCCAL
  adjL=OSCCAL; //L  
 
  temp=0x3c00+adjL;
  MC321_IRC_INST(temp);
  temp=0x5600+0x1fa;//0x1fa opt1 L地址
  MC321_IRC_INST(temp); //option1 L
    
  MC321_IRC_INST(0x0000);
 
  //----------------------------
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
  IRC_VALUE= (u16)((cnt[0]+cnt[1]+cnt[2])/3 );
}

void MC32T8132FTtest(u8 IRC_Option)
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
    TIM_Cmd(TIM3,ENABLE);//TIMER3一旦使能 TIM3_CH2/PC7就输出2V电压  可能STM32初始化程序还需要优化
    while(TIM_GetITStatus(TIM3,TIM_IT_Update) == RESET) ;
    cnt[i] = TIM_GetCounter(TIM4);
  }
  IRC_VALUE= (u16)((cnt[0]+cnt[1]+cnt[2])/3 );
}

//
u8 ChangeTempIRC_8132(u8 TADJ_temp,u16 IRC_FreqType,u16 IRC_FreqMin,u16 IRC_FreqMax)
{
  u8 i;
  u16 temp0_FT,temp1_FT;
  u8 OSCCAL_temp=0x80;//频率校准位先给中间值
  OPTION_FRT=0; //initial 
  for(i=0;i<8;i++)
  {
    MC32T8132FTtest_8Bit(OSCCAL_temp,TADJ_temp);
    //以下判断频率是否满足要求
    if (IRC_VALUE == IRC_FreqType)
    {
      break;
    }
    //以下不满足做处理 并继续校准
    if (IRC_VALUE<IRC_FreqType)
    {
      OSCCAL_temp |=0x80 >>i;
    } 
    else
    {
      OSCCAL_temp &=~(0x80>>i);
    }
    OSCCAL_temp |=0x40>>i;
  }
  MC32T8132FTtest_8Bit(OSCCAL_temp,TADJ_temp+1);
  temp0_FT=IRC_VALUE;

  MC32T8132FTtest_8Bit(OSCCAL_temp,TADJ_temp);
  temp1_FT=IRC_VALUE;

  if(temp0_FT>IRC_FreqType)
  {
    temp0_FT = temp0_FT-IRC_FreqType;
  }
  else
  {
    temp0_FT = IRC_FreqType-temp0_FT;
  }
  if(temp1_FT>IRC_FreqType)
  {
    temp1_FT = temp1_FT-IRC_FreqType;
  }
  else
  {
    temp1_FT = IRC_FreqType - temp1_FT;
  }
  
  if(temp0_FT<temp1_FT)
  {
    OSCCAL_temp=OSCCAL_temp+1;
    IRC_VALUE=temp0_FT;
  }       
  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    OSCCAL = OSCCAL_temp;
    TADJ = TADJ_temp;
    return 1;//校准成功 
  } 
  return 0;
}
//--------------------------------------------------------
/*
u8 MC32T8132IRC_8Bit(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 j,optfa;
  u16 Fdata;
  u8 temp;
  
  //power off then power on & model in
  MC32T8132_MODEL_IN(vdd50);
  
  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID) 
  {
    ERORR_VALUE=IRC_ModeIn_false;
    return 0;
  }

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
  
  OPTION_TEMP = optfa;//保存该值 后面校准时改变温度值时要用到
  OPTION_TEMP<<=8;
  
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
//  MC321_IRC_INST(0x3c00+0x0e);
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

  //开始校验
  //先用OPTION_TEMPADJ_1进行第一次校验
  
  if(ChangeTempIRC_8132(0x07,IRC_FreqType,IRC_FreqMin,IRC_FreqMax))
  {
    return 1;
  }
  //用OPTION_TEMPADJ_1校验失败，再用OPTION_TEMPADJ_1进行第二次校验
  if(ChangeTempIRC_8132(0x06,IRC_FreqType,IRC_FreqMin,IRC_FreqMax))
  {
    return 1;
  }    
  //用OPTION_TEMPADJ_2校验失败，再用OPTION_TEMPADJ_1进行第三次校验
  if(ChangeTempIRC_8132(0x08,IRC_FreqType,IRC_FreqMin,IRC_FreqMax))
  {
    return 1;
  }    
  
  //三次校验都失败  
  OSCCAL=0xff;
  TADJ=0xff;
  ERORR_VALUE=IRC_Value_false;
  return 0;
}
*/
//正式版
u8 MC32T8132IRC_8Bit(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i,j,optfa;
  u16 temp0_FT,temp1_FT;
  u8 mm=0x80;
  
  MC301_Instruct(TestMod);    //进入测试模式    
  for(j=0;j<5;j++)
  {
      MC321_IRC_INST(0X0000); //5 NOP
  }
  
  //以下设置OPTION寄存器 
  //opt0 H
  MC321_IRC_INST(0x3c00+0x80);        //movai 0x80    //FOSC=00
  MC321_IRC_INST(0x57f9);             //movra 0x1f9
  
  MC321_IRC_INST(0x3c00+0x70);        //movai	0x60    //128分频
  MC321_IRC_INST(0x57f8);             //movra	0x1f8  
  
  //OPTION2
  //opt2 H
  MC321_IRC_INST(0x3c00+0x80);        //movai   optfa   //VDSEL=1（1.9V）
  MC321_IRC_INST(0x57fd);             //movra	0x1fd
  
//  mm=0x0080; //8位频率校准位256 中间值128

  MC321_IRC_INST(0x3c01);//设置测试模式为 高频系统时钟由PSDO输出     
  MC321_IRC_INST(0x57f7); 
  MC321_IRC_INST(0X0000);
  
  for(j=0;j<3;j++)
  {
    if(j==0)
    {
      optfa = 0x8e;//0x07
      TADJ = 0x07;
    }
    else if(j==1)
    {
      optfa = 0x8c;//0x06
      TADJ = 0x06;
    }
    else if(j==2)
    {
      optfa = 0x90;//0x08
      TADJ = 0x08;
    }
    //opt2 H
    MC321_IRC_INST(0x3c00+optfa);       //movai   optfa 
    MC321_IRC_INST(0x57fd);             //movra	0x1fd
  
    OPTION_FRT=0; //initial 
    for(i=0;i<8;i++)
    {
      MC32T8132FTtest(mm);
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
    MC32T8132FTtest(mm+1);
    temp0_FT=IRC_VALUE;

    MC32T8132FTtest(mm);
    temp1_FT=IRC_VALUE;

    if((temp0_FT-IRC_FreqType)<(IRC_FreqType-temp1_FT))
    {
      mm=mm+1;
      IRC_VALUE=temp0_FT;
    }       
    if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
    {
      OSCCAL=mm;
      return 1;//校准成功 
    } 
  }
  OSCCAL=0xff;
  TADJ=0xff;
  return 0;
}


//正式版
u8 MC32T8132IRC_8Bit_Check(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 optfa;
  u8 mm=0x80;
  
  if(TADJ==0x07)
  {
    optfa = 0x8e;//0x07
  }
  else if(TADJ==0x06)
  {
    optfa = 0x8c;//0x06
  }
  else if(TADJ == 0x08)
  {
    optfa = 0x90;//0x08
  }
  //opt2 H
  MC321_IRC_INST(0x3c00+optfa);       //movai   optfa 
  MC321_IRC_INST(0x57fd);             //movra	0x1fd

  mm=OSCCAL;
  MC32T8132FTtest(mm);

  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    //OSCCAL=mm;
    return 1;//校准成功 
  } 
  
  OSCCAL=0xff;
  TADJ=0xff;
  return 0;
}

//正式版
u8 MC32T8132IRC_8Bit_ModeInCheck(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 j,optfa;
  u8 mm=0x80;
  
  MC301_Instruct(TestMod);    //进入测试模式    
  for(j=0;j<5;j++)
  {
      MC321_IRC_INST(0X0000); //5 NOP
  }
  
  //以下设置OPTION寄存器 
  //opt0 H
  MC321_IRC_INST(0x3c00+0x80);        //movai 0x80    //FOSC=00
  MC321_IRC_INST(0x57f9);             //movra 0x1f9
  
  MC321_IRC_INST(0x3c00+0x70);        //movai	0x60    //128分频
  MC321_IRC_INST(0x57f8);             //movra	0x1f8  
  
  //OPTION2
  //opt2 H
  MC321_IRC_INST(0x3c00+0x80);        //movai   optfa   //VDSEL=1（1.9V）
  MC321_IRC_INST(0x57fd);             //movra	0x1fd
  
//  mm=0x0080; //8位频率校准位256 中间值128

  MC321_IRC_INST(0x3c01);//设置测试模式为 高频系统时钟由PSDO输出     
  MC321_IRC_INST(0x57f7); 
  MC321_IRC_INST(0X0000);
  
  if(TADJ==0x07)
  {
    optfa = 0x8e;//0x07
  }
  else if(TADJ==0x06)
  {
    optfa = 0x8c;//0x06
  }
  else if(TADJ == 0x08)
  {
    optfa = 0x90;//0x08
  }
  //opt2 H
  MC321_IRC_INST(0x3c00+optfa);       //movai   optfa 
  MC321_IRC_INST(0x57fd);             //movra	0x1fd

  mm=OSCCAL;
  MC32T8132FTtest(mm);

  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    //OSCCAL=mm;
    return 1;//校准成功 
  } 
  
  OSCCAL=0xff;
  TADJ=0xff;
  return 0;

}


/*******************************************************************************
* Function Name  : MC32T8132_Program
* Description    : MC32T8132有擦除功能，先判断本次操作是不是擦除，如果是擦除，就
*                  只进行擦除操作；是烧写程序，就不进行擦除操作，只烧写
* Input          : None
* Output         : None
* Return         : 1 :成功  0：失败
*******************************************************************************/
u8 MC32T8132_Program()
{
  u8    ReCnt=0;
  u16   Fdata,ROMReadData;
  u8	FdataL,FdataH;  
  u8    ReFlag=0;
  u16   CheckSum=0;
  u16   mcu_addr;   
  u32   op_addr;
  u16   erase_num;//要擦
  u16    erase_en;//
  u8    temp;
  u16   temp1;
  u8    i;
  
  u8    id_length=0;
  u16   id_addr0,id_addr1,id_addr2,id_addr3;
  u32   id_end,current_id,max_id;  
  
  u8    j;
  u8    ForceReCnt=0;  
  u8    SuccessCnt;   
  u16   Addres;   
  
  u8	option_shift=0; 
  
  WriteConfig_xx.RomBit=0xffff;
  WriteConfig_xx.ProTime=0;
  WriteConfig_xx.ReadProAddrAndData=1;
  WriteConfig_xx.Page=0;
  WriteConfig_xx.Addr_Flash_OptionStart=Addr_Flash_OptionProtec;
  WriteConfig_xx.SecurityOption=1;
  WriteConfig_xx.SecurityBit=15;
  WriteConfig_xx.WriteCheckNeed=0;
  WriteConfig_xx.ReadCnt=1;
  WriteConfig_xx.ReadOkCnt=1;
  WriteConfig_xx.ReWriteCnt=3;
  WriteConfig_xx.ForceWriteCnt=1;
  WriteConfig_xx.AllCheckNeed=1;  
  
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
  
  if(erase_en&0x0100)
  {
    WriteFlag = 1;//在线烧录
  }
  else
  {
    WriteFlag = 0;//裸片烧录
  }  
  
  
//------------------------------进模式----------------------------  
  MC32T8132_MODEL_IN(vdd50);
  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID) 
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  }
 
//------------------------------擦除----------------------------   
  temp = erase_en & 0x0006;
  if(temp == 0x00 || temp == 0x06)//需要擦除
  {//和PC通讯协议是 ：00-擦除并编程；01-只编程：11-只擦除
    temp = (((u8)erase_en) & 0x01)<<4;//擦为1或0
    if(erase_num == 0x00)//全擦 即擦除未锁住的块
    {
      ReFlag = MC32T8132_ERASE(0x20+temp);//全擦为1或0
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
          ReFlag = MC32T8132_ERASE(0x00+temp+i);//块擦为1或0
          if(ReFlag == 1)
          {
            //power off then power on & model in
            MC32T8132_MODEL_IN(vdd50);  
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
  
//------------------------------IRC校准----------------------------     
  if (DeviceConfig_xx.IRC_Select==0x01)//正式代码
//  if (DeviceConfig_xx.IRC_Select==0x00)//调试代码
  {
    ReFlag=MC32T8132IRC_8Bit(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
    if(ReFlag==1)
    {
      ReFlag=MC32T8132IRC_8Bit_Check(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
    }
    ConfigTIM4();//关闭定时器TIMER3 TIMER4
    if(ReFlag == 0) 
    {
      ERORR_VALUE=IRC_Value_false;
      return 0;
    }
  }

//------------------------------进模式----------------------------  
  MC32T8132_MODEL_IN(vdd50);
  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID) 
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  }
  
//------------------------------Write ROM----------------------------   
  if (DeviceConfig_xx.WriteEn & 0x0f)//
  {
    CheckSum=0;
    FM_CS_0;
    FMReadMore(Addr_Flash_ROMStart+DeviceConfig_xx.RomFirAddr-1);
    Fdata =Rxdata;
    
    //MCU的芯片才可能会有滚动码这个需求    其他芯片不需要关于iic_data的代码
    id_length=iic_data[1];
    id_addr0=(iic_data[2]<<8)+iic_data[3];// ADDRL
    id_addr1=(iic_data[4]<<8)+iic_data[5];
    id_addr2=(iic_data[6]<<8)+iic_data[7];
    id_addr3=(iic_data[8]<<8)+iic_data[9];//ADDRH
    current_id=(iic_data[10]<<24)+(iic_data[11]<<16)+(iic_data[12]<<8)+iic_data[13];//当前已经烧写过的芯片写入的编号
    id_end=(iic_data[15]<<24)+(iic_data[16]<<16)+(iic_data[17]<<8)+iic_data[18];//烧写成功的次数 限制值
    max_id=(iic_data[27]<<24)+(iic_data[28]<<16)+(iic_data[29]<<8)+iic_data[30]; //max-id//芯片最大编号
    if (current_id>max_id)
    {
      iic_data[10]=iic_data[23];
      iic_data[11]=iic_data[24];
      iic_data[12]=iic_data[25];
      iic_data[13]=iic_data[26];
    }
    if (iic_data[0]==0x0a)//烧写次数限制功能打开
    {
      if (OKcounter>=id_end)//烧写成功的次数达到限制值
      {
        ERORR_VALUE=OK_Counter_Full;
        return 0; // full limit
      }
    }
    
    
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
      
      if (iic_data[0]==0x0a) //id enable MCU的芯片才可能会有滚动码这个需求
      {
        if (mcu_addr==id_addr0)
        {
          //Fdata=iic_data[13];
          Fdata=FdataH*0x100+iic_data[13]; // must keep the H byte for code
        }
        else if ((mcu_addr==id_addr1)&& (id_length>1))
        {
          //Fdata=iic_data[12];
          Fdata=FdataH*0x100+iic_data[12];
        }
        else if ((mcu_addr==id_addr2)&& (id_length>2))
        {
          //Fdata=iic_data[11];
          Fdata=FdataH*0x100+iic_data[11];
        }
        else if ((mcu_addr==id_addr3)&&(id_length>3))
        {
          //Fdata=iic_data[10];
          Fdata=FdataH*0x100+iic_data[10];
        }
      }
      
      ReCnt=WriteConfig_xx.ReWriteCnt;
      while(ReCnt--)  //start wr rom
      {	 
        ForceReCnt=WriteConfig_xx.ForceWriteCnt;
        //if (Fdata!=0xffff)
        //{
          while(ForceReCnt--)
          {
            MC321_W(Fdata); //wr data to rom
          }
        //}
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
        if (DeviceConfig_xx.MCU_ID==0x8132)
        {
          mcu_addr = MC321_PreRead(PAddrRe);
          if (mcu_addr!=OTP_ADDR/2)
          {
            ERORR_VALUE=ADDR_EER;
            return 0;
          }
        }
      }      
    }
    FM_CS_1;
  }

//------------------------------Write OPTION ---------------------------- 
  if(DeviceConfig_xx.WriteEn & 0xf0)
  {
    FM_CS_0;
    FMReadMore(Addr_Flash_OptionProtec-1);
    Fdata =Rxdata;
    
    for(op_addr=0;op_addr<DeviceConfig_xx.OptionSize;op_addr++)//
    {
      SPI_ReadByte();//从flash中取一个字节
      FdataL=Rxdata;
      SPI_ReadByte();//再从flash中取一个字节
      FdataH=Rxdata;
      Fdata=FdataH*0x100+FdataL;//组成一个字
      
      //option0 
      if(op_addr==0)
      {          
        Fdata|=0x8000;//不写加密位
      }      
      //option1 
      if(op_addr==1)
      {
        Fdata = (Fdata&0xff00) + (OSCCAL&0x00ff);       //校准得到的频率校准值
      }
      //option2                 //根据测试结果 正式产品需要写死                                                
      if(op_addr==2)//温度校准位 测试阶段 根据上位机写  正式版本写校准得到的值
      {          
        temp1 = TADJ;
        temp1 = temp1<<9;
        Fdata = (Fdata&0x81ff)+temp1;
        
        ROMReadData = MC321_ReadByte(0x800b);
        ROMReadData = ROMReadData&0x001f;//取0x800b低5位
        Fdata = (Fdata&0xffe0)+ROMReadData;      
        
        ROMReadData=MC321_ReadByte(0x8002); //set ADDR
      }
      //option3 //
      if(op_addr==3)//取成测写入的值 
      {          
        
        ROMReadData = MC321_ReadByte(0x800c);
        ROMReadData = ROMReadData&0x007f;//取0x800b低7位
        Fdata = (Fdata&0xff80)+ROMReadData;  
        
        ROMReadData = MC321_ReadByte(0x800d);
        ROMReadData = ROMReadData&0x01ff;//取0x800d低9位
        ROMReadData = ROMReadData<<7;
        Fdata = (Fdata&0x007f)+ROMReadData;   
        
        ROMReadData=MC321_ReadByte(0x8003); //set ADDR
      }
      
      WriteConfig_xx.OptionValue[op_addr] = Fdata;//烧option之前先把值存起来，以方便后续全面校验时读值
      WriteConfig_xx.OptionAddr[op_addr]=DeviceConfig_xx.OptionAddr+op_addr+option_shift;
      ReCnt=WriteConfig_xx.ReWriteCnt;
      while(ReCnt--)  
      {	
        ForceReCnt=WriteConfig_xx.ForceWriteCnt;
        if (Fdata != 0xffff)  // 
        {
          while(ForceReCnt--)
          {
            MC321_WriteByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift,Fdata);
          }
        }        
	SuccessCnt = 0;
	if (WriteConfig_xx.WriteCheckNeed)
	{
	  for(i=0;i<WriteConfig_xx.ReadCnt;i++)
	  {
	    ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);         
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
  
//------------------------------频率验证 取写入芯片的温度校准值和频率校准值----------------------------  
  if (DeviceConfig_xx.IRC_Select==0x01)//正式代码
  {
    ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+2+option_shift); //set ADDR
    TADJ=ROMReadData/256;
    TADJ>>=1;
    TADJ&=0x3f;
    ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+1+option_shift); 
    OSCCAL=ROMReadData%256;
    ReFlag=MC32T8132IRC_8Bit_ModeInCheck(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
    ConfigTIM4();//关闭定时器TIMER3 TIMER4
    if(ReFlag == 0) 
    {
      ERORR_VALUE=IRC_Value_false;
      return 0;
    }
  }
//------------------------------进模式----------------------------  
  MC32T8132_MODEL_IN(vdd50);
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
 
      //MCU的芯片才可能会有滚动码这个需求    其他芯片不需要关于iic_data的代码
      id_length=iic_data[1];
      id_addr0=(iic_data[2]<<8)+iic_data[3];// ADDRL
      id_addr1=(iic_data[4]<<8)+iic_data[5];
      id_addr2=(iic_data[6]<<8)+iic_data[7];
      id_addr3=(iic_data[8]<<8)+iic_data[9];//ADDRH
      current_id=(iic_data[10]<<24)+(iic_data[11]<<16)+(iic_data[12]<<8)+iic_data[13];//当前已经烧写过的芯片写入的编号
      id_end=(iic_data[15]<<24)+(iic_data[16]<<16)+(iic_data[17]<<8)+iic_data[18];//烧写成功的次数 限制值
      max_id=(iic_data[27]<<24)+(iic_data[28]<<16)+(iic_data[29]<<8)+iic_data[30]; //max-id//芯片最大编号
      if (current_id>max_id)
      {
        iic_data[10]=iic_data[23];
        iic_data[11]=iic_data[24];
        iic_data[12]=iic_data[25];
        iic_data[13]=iic_data[26];
      }
      if (iic_data[0]==0x0a)//烧写次数限制功能打开
      {
        if (OKcounter>=id_end)//烧写成功的次数达到限制值
        {
          ERORR_VALUE=OK_Counter_Full;
          return 0; // full limit
        }
      }
      
      ROMReadData=MC321_ReadByte(0x0000); //set ADDR
      for(OTP_ADDR=DeviceConfig_xx.RomFirAddr;OTP_ADDR<=DeviceConfig_xx.RomEndAddr;)
      {
	SPI_ReadByte();
	FdataL=Rxdata;
	SPI_ReadByte();
	FdataH=Rxdata;
	Fdata=FdataH*0x100+FdataL;
	
	mcu_addr=OTP_ADDR/2;
	OTP_ADDR=OTP_ADDR+2;
        
        if (iic_data[0]==0x0a) //id enable MCU的芯片才可能会有滚动码这个需求
        {
          if (mcu_addr==id_addr0)
          {
            //Fdata=iic_data[13];
            Fdata=FdataH*0x100+iic_data[13]; // must keep the H byte for code
          }
          else if ((mcu_addr==id_addr1)&& (id_length>1))
          {
            //Fdata=iic_data[12];
            Fdata=FdataH*0x100+iic_data[12];
          }
          else if ((mcu_addr==id_addr2)&& (id_length>2))
          {
            //Fdata=iic_data[11];
            Fdata=FdataH*0x100+iic_data[11];
          }
          else if ((mcu_addr==id_addr3)&&(id_length>3))
          {
            //Fdata=iic_data[10];
            Fdata=FdataH*0x100+iic_data[10];
          }
        }
	
	if (WriteConfig_xx.AllCheckNeed == 2)
	{
          for(i=1;i<3;i++)
          {
            SuccessCnt = 0;
            for(j=0;j<WriteConfig_xx.ReadCnt;j++)
            {
              ROMReadData=MC321_R_DiffMode(i);
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
          }
          else
          {
            OTP_ADDR= mcu_addr;
            return 0;
          }
	}
	else
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
          }
          else
          {
            ERORR_VALUE=NOMALL_ERROR;
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
  }
  
//------------------------------option全面读校验----------------------------    
  if(DeviceConfig_xx.WriteEn & 0xf0)
  {
    if(WriteConfig_xx.AllCheckNeed > 0)
    {
      for(op_addr=0;op_addr<DeviceConfig_xx.OptionSize;op_addr++)
      {
	Fdata=WriteConfig_xx.OptionValue[op_addr];
        Addres=WriteConfig_xx.OptionAddr[op_addr];

	if (WriteConfig_xx.AllCheckNeed == 2)
	{
          for(i=1;i<3;i++)
          {
            SuccessCnt = 0;
            for(j=0;j<WriteConfig_xx.ReadCnt;j++)
            {
              ROMReadData=MC321_ReadByte_DiffMode(i,Addres);
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
              if(i==1)
              {
                ERORR_VALUE=OPTION_MAGRIN1_ERROR;
              }
              else if(i==2)//测试用
              {
                ERORR_VALUE=OPTION_OFF_MAGRIN_ERROR;
              }
              break;
            }
          }
          if (i==3)//3种模式读取都和写入的数据一致 烧写成功 2
          {
            
          }
          else
          {
            OTP_ADDR= Addres;
            return 0;
          } 
	}
	else
        {
          SuccessCnt = 0;
	  for(j=0;j<WriteConfig_xx.ReadCnt;j++)
	  {
	    ROMReadData=MC321_ReadByte(Addres);
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
  }
  
//------------------------------write protect bit----------------------------    
  if(DeviceConfig_xx.WriteEn & 0xf0)
  {  
    FM_CS_0;
    FMReadMore(Addr_Flash_Option-1);
    Fdata =Rxdata;
    SPI_ReadByte();//从flash中取一个字节
    FdataL=Rxdata;
    SPI_ReadByte();//再从flash中取一个字节
    FdataH=Rxdata;
    Fdata=FdataH*0x100+FdataL;//组成一个字 

    ReCnt=WriteConfig_xx.ReWriteCnt;
    while(ReCnt--) 
    {	
      ForceReCnt=WriteConfig_xx.ForceWriteCnt;      
      if (Fdata != 0xffff)  // 
      {
        while(ForceReCnt--)
        {
          MC321_WriteByte(DeviceConfig_xx.OptionAddr+option_shift,Fdata);
        } 
      } 
      if (WriteConfig_xx.AllCheckNeed == 2)
      {
        for(i=1;i<3;i++)
        {
          SuccessCnt = 0;
          for(j=0;j<WriteConfig_xx.ReadCnt;j++)
          {
            ROMReadData=MC321_ReadByte_DiffMode(i,DeviceConfig_xx.OptionAddr+option_shift);
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
            if(i==1)
            {
              ERORR_VALUE=OPTION_MAGRIN1_ERROR;
            }
            else if(i==2)//测试用
            {
              ERORR_VALUE=OPTION_OFF_MAGRIN_ERROR;
            }
            break;
          }
        }
        if (i==3)//3种模式读取都和写入的数据一致 烧写成功 2
        {
          break;
        }
        else
        {
          OTP_ADDR= DeviceConfig_xx.OptionAddr+option_shift;
          return 0;
        } 
      }  
      else if(WriteConfig_xx.AllCheckNeed == 1)
      {
        SuccessCnt = 0;
        for(j=0;j<WriteConfig_xx.ReadCnt;j++)
        {
          ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+option_shift);
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