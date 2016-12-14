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
  VDD65V_Off;//��Ҫ�������ֵ�λ�����ڳ�����5V,��U10������6.5V�ص�  
  POWER_OFF(vpp00,vdd00);
//  Delay_1ms(50);//10uf �ϵ����� VDD��VPP������Ҫ50ms
//  Delay_1ms(500);//100uf �ϵ����� VDD��VPP������Ҫ>500ms
//  Delay_1ms(1200);//470uf �ϵ����� VDD��VPP������Ҫ500ms
  if(WriteFlag)//������¼
  {
    Delay_1ms(1200);
  }
  else//��Ƭ��¼
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
//  Delay_100us(3);
//  Delay_100us(30);
//  Delay_100us(300); 
   if(WriteFlag)//������¼
  {
    Delay_100us(300);
  }
  else//��Ƭ��¼
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
  Delay_1ms(300);  
}
/*******************************************************************************
* Function Name  : MC32T8132_ERASE
* Description    : ȫ����ѡ��ĳһ����в���
* Input          : OTPData 0x20:ȫ��Ϊ0��0x30ȫ��Ϊ1��0x20+num������num��Ϊ0��0x30+num������num��Ϊ1
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
    while((temp&0x0001) != 0x0001 )//�ȴ�������� �����ɹ� ����δ��ʱ
    {
      Delay_1ms(100);
      temp = MC321_ReadByte(0XFFFE);
      
      wait_time++;
      if(wait_time>60)//
      {
        wait_time = 0;        
        break;//����ʧ��
      }
    }
    if(temp == 0x0009)
    {
      return 1;//�����ɹ� 
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
  u8  adjL,adjH;
  u16 i;
  u16 cnt[3];
  
  //OPTION 2��
  //bit14:9 --  bit5:bit0 TADJ  
/*  
  adjH =TADJ;
  adjH = adjH<<1;
  adjH = adjH&0x7e;
  adjH = adjH+(OPTION_TEMP&0x8000)/256;//

  temp=0x3c00+adjH;
  MC321_IRC_INST(temp);
  temp=0x5600+0x1fd;//0x1fd opt2 H��ַ
  MC321_IRC_INST(temp); //option2 H
  MC321_IRC_INST(0x0000);
*/ 
  //OPTION 1��
  //bit7:0  --  bit7:0   OSCCAL
  adjL=OSCCAL; //L  
  
 // MC321_IRC_INST(0x3c00+IRC_Option);
 // MC321_IRC_INST(0x57fa);//���ܲ���ģʽ�£�Ƶ��У׼λ��Ӧ�ļĴ�����ַ0x1fa
 
  temp=0x3c00+adjL;
  MC321_IRC_INST(temp);
  temp=0x5600+0x1fa;//0x1fa opt1 L��ַ
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
  MC321_IRC_INST(0x57fa);//���ܲ���ģʽ�£�Ƶ��У׼λ��Ӧ�ļĴ�����ַ0x1fa
  
//  MC321_IRC_INST(0x3c01);//���ò���ģʽΪ ��Ƶϵͳʱ����PSDO���     
//  MC321_IRC_INST(0x57f7);  
  
  MC321_IRC_INST(0x0000);//�����ֲ���ָ����ÿ��ָ�������������һ��ָ��ʱ�����������
//  MC321_IRC_INST(0x0000);
//  MC321_IRC_INST(0x0000);
  Delay_1ms(1);//��Ҫ����ʱʱ�䰴оƬʵ�ʲ���Ч������ Ҫ���ǵȲ����ȶ����Ƶ�� 
  for (i=0;i<3;i++)
  {
    TIM_SetCounter(TIM3,0);
    TIM_SetCounter(TIM4,0);
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
    TIM_Cmd(TIM4,ENABLE);
    TIM_Cmd(TIM3,ENABLE);//TIMER3һ��ʹ�� TIM3_CH2/PC7�����2V��ѹ  ����STM32��ʼ��������Ҫ�Ż�
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
  u8 OSCCAL_temp=0x80;//Ƶ��У׼λ�ȸ��м�ֵ
  OPTION_FRT=0; //initial 
  for(i=0;i<8;i++)
  {
    MC32T8132FTtest_8Bit(OSCCAL_temp,TADJ_temp);
    //�����ж�Ƶ���Ƿ�����Ҫ��
    if (IRC_VALUE == IRC_FreqType)
    {
      break;
    }
    //���²����������� ������У׼
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
    return 1;//У׼�ɹ� 
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

  MC301_Instruct(TestMod);    //�������ģʽ    
  for(j=0;j<5;j++)
  {
      MC321_IRC_INST(0X0000); //5 NOP
  }
    
  //�ȴ�flash�ж�PC��OPTION2��HIRC��ѹУ׼λ(BIT15)��HIRC�¶�У׼λ(BIT14:9) 
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
  
  OPTION_TEMP = optfa;//�����ֵ ����У׼ʱ�ı��¶�ֵʱҪ�õ�
  OPTION_TEMP<<=8;
  
  //��������OPTION�Ĵ��� 
  //OPTION0
  MC321_IRC_INST(0x3c00+0x80);        //movai 0x80    //��ʹ�ܴ������λ
  MC321_IRC_INST(0x57f9);             //movra 0x1f9
  MC321_IRC_INST(0x3c00+0x70);        //movai	0x60    //128��Ƶ
  MC321_IRC_INST(0x57f8);             //movra	0x1f8  
  //OPTION1
  MC321_IRC_INST(0x3c00+0x00);        //movai 0x00 
  MC321_IRC_INST(0x57fb);             //movra 0x1fb
  MC321_IRC_INST(0x3c00+0x00);        //movai 0x00
  MC321_IRC_INST(0x57fa);             //movra	0x1fa
  
  //OPTION2
  //HIRC��ѹУ׼λ��HIRC�¶�У׼λ
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
  
//  mm=0x0080; //8λƵ��У׼λ256 �м�ֵ128
  MC321_IRC_INST(0x3c01);//���ò���ģʽΪ ��Ƶϵͳʱ����PSDO���     
  MC321_IRC_INST(0x57f7); 
  MC321_IRC_INST(0X0000);

  //��ʼУ��
  //����OPTION_TEMPADJ_1���е�һ��У��
  
  if(ChangeTempIRC_8132(0x07,IRC_FreqType,IRC_FreqMin,IRC_FreqMax))
  {
    return 1;
  }
  //��OPTION_TEMPADJ_1У��ʧ�ܣ�����OPTION_TEMPADJ_1���еڶ���У��
  if(ChangeTempIRC_8132(0x06,IRC_FreqType,IRC_FreqMin,IRC_FreqMax))
  {
    return 1;
  }    
  //��OPTION_TEMPADJ_2У��ʧ�ܣ�����OPTION_TEMPADJ_1���е�����У��
  if(ChangeTempIRC_8132(0x08,IRC_FreqType,IRC_FreqMin,IRC_FreqMax))
  {
    return 1;
  }    
  
  //����У�鶼ʧ��  
  OSCCAL=0xff;
  TADJ=0xff;
  ERORR_VALUE=IRC_Value_false;
  return 0;
}
*/
//��ʽ��
u8 MC32T8132IRC_8Bit(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i,j,optfa;
  u16 temp0_FT,temp1_FT;
  u8 mm=0x80;
  u16 Fdata;
  
  //power off then power on & model in
  MC32T8132_MODEL_IN(vdd50);
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
  MC301_Instruct(TestMod);    //�������ģʽ    
  for(j=0;j<5;j++)
  {
      MC321_IRC_INST(0X0000); //5 NOP
  }
  
  //��������OPTION�Ĵ��� 
  //opt0 H
  MC321_IRC_INST(0x3c00+0x80);        //movai 0x80    //FOSC=00
  MC321_IRC_INST(0x57f9);             //movra 0x1f9
  
  MC321_IRC_INST(0x3c00+0x70);        //movai	0x60    //128��Ƶ
  MC321_IRC_INST(0x57f8);             //movra	0x1f8  
  
  //OPTION2
  //opt2 H
  MC321_IRC_INST(0x3c00+0x80);        //movai   optfa   //VDSEL=1��1.9V��
  MC321_IRC_INST(0x57fd);             //movra	0x1fd
  
//  mm=0x0080; //8λƵ��У׼λ256 �м�ֵ128

  MC321_IRC_INST(0x3c01);//���ò���ģʽΪ ��Ƶϵͳʱ����PSDO���     
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
//  mm=0x0080; //8λƵ��У׼λ256 �м�ֵ128
/*
    MC321_IRC_INST(0x3c01);//���ò���ģʽΪ ��Ƶϵͳʱ����PSDO���     
    MC321_IRC_INST(0x57f7); 
    MC321_IRC_INST(0X0000);
*/      
    OPTION_FRT=0; //initial 
    for(i=0;i<8;i++)
    {
      MC32T8132FTtest(mm);
      //�����ж�Ƶ���Ƿ�����Ҫ��
      if (IRC_VALUE == IRC_FreqType)
      {
        break;
      }
      //���²����������� ������У׼
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
      return 1;//У׼�ɹ� 
    } 
  }
  OSCCAL=0xff;
  TADJ=0xff;
  ERORR_VALUE=IRC_Value_false;
  return 0;
/*
  optfa = optfa&0x80+0x0e;//0x07
  TADJ = 0x07;
//  optfa = 0x0e;
//  MC321_IRC_INST(0x3c00+optfa);  //movai   optfa 
  MC321_IRC_INST(0x3c00+optfa);  //movai   optfa
  MC321_IRC_INST(0x57fd);             //movra	0x1fd
  
//  mm=0x0080; //8λƵ��У׼λ256 �м�ֵ128
  MC321_IRC_INST(0x3c01);//���ò���ģʽΪ ��Ƶϵͳʱ����PSDO���     
  MC321_IRC_INST(0x57f7); 
  MC321_IRC_INST(0X0000);
  
  OPTION_FRT=0; //initial 
  for(i=0;i<8;i++)
  {
    MC32T8132FTtest(mm);
    //�����ж�Ƶ���Ƿ�����Ҫ��
    if (IRC_VALUE == IRC_FreqType)
    {
      break;
    }
    //���²����������� ������У׼
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
    OPTION_FRT=mm;
    return 1;//У׼�ɹ� 
  } 
 
  optfa = optfa&0x80+0x0c;//0x06
  TADJ = 0x06;
  MC321_IRC_INST(0x3c00+optfa);  //movai   optfa 
  MC321_IRC_INST(0x57fd);             //movra	0x1fd
  OPTION_FRT=0; //initial 
  for(i=0;i<8;i++)
  {
    MC32T8132FTtest(mm);
    //�����ж�Ƶ���Ƿ�����Ҫ��
    if (IRC_VALUE == IRC_FreqType)
    {
      break;
    }
    //���²����������� ������У׼
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
    OPTION_FRT=mm;
    return 1;//У׼�ɹ� 
  } 

 
  optfa = optfa&0x80+0x10;//0x08
  TADJ = 0x08;
  MC321_IRC_INST(0x3c00+optfa);  //movai   optfa 
  MC321_IRC_INST(0x57fd);             //movra	0x1fd
  OPTION_FRT=0; //initial 
  for(i=0;i<8;i++)
  {
    MC32T8132FTtest(mm);
    //�����ж�Ƶ���Ƿ�����Ҫ��
    if (IRC_VALUE == IRC_FreqType)
    {
      break;
    }
    //���²����������� ������У׼
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
    OPTION_FRT=mm;
    return 1;//У׼�ɹ� 
  } 
 
  OPTION_FRT=0XFF;
  return 0;//У׼ʧ�� 
 */
}
//IRC���԰�
u8 MC32T8132IRC(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i,j,optfa;
  u16 temp0_FT,temp1_FT;
  u8 mm=0x80;
  u16 Fdata;
  
  //power off then power on & model in
  MC32T8132_MODEL_IN(vdd50);
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
  MC301_Instruct(TestMod);    //�������ģʽ    
  for(j=0;j<5;j++)
  {
      MC321_IRC_INST(0X0000); //5 NOP
  }
  
  //�ȴ�flash�ж�PC��OPTION2��HIRC��ѹУ׼λ(BIT15)��HIRC�¶�У׼λ(BIT14:9) 
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
  
  //��������OPTION�Ĵ��� 
  //OPTION0
  MC321_IRC_INST(0x3c00+0x80);        //movai 0x80    //��ʹ�ܴ������λ
  MC321_IRC_INST(0x57f9);             //movra 0x1f9
  MC321_IRC_INST(0x3c00+0x70);        //movai	0x60    //128��Ƶ
  MC321_IRC_INST(0x57f8);             //movra	0x1f8  
  //OPTION1
  MC321_IRC_INST(0x3c00+0x00);        //movai 0x00 
  MC321_IRC_INST(0x57fb);             //movra 0x1fb
  MC321_IRC_INST(0x3c00+0x00);        //movai 0x00
  MC321_IRC_INST(0x57fa);             //movra	0x1fa
  //OPTION2
  //HIRC��ѹУ׼λ��HIRC�¶�У׼λ
  MC321_IRC_INST(0x3c00+0x00+optfa);  //movai   optfa 
  MC321_IRC_INST(0x57fd);             //movra	0x1fd
  MC321_IRC_INST(0x3c00+0x00);        //movai	0x00
  MC321_IRC_INST(0x57fc);             //movra	0x1fc
  //OPTION3
  MC321_IRC_INST(0x3c00+0x00);        //movai 0x00 
  MC321_IRC_INST(0x57ff);             //movra 0x1ff
  MC321_IRC_INST(0x3c00+0x00);        //movai 0x00
  MC321_IRC_INST(0x57fe);             //movra	0x1fe
  
//  mm=0x0080; //8λƵ��У׼λ256 �м�ֵ128
  MC321_IRC_INST(0x3c01);//���ò���ģʽΪ ��Ƶϵͳʱ����PSDO���     
  MC321_IRC_INST(0x57f7); 
  MC321_IRC_INST(0X0000);
  OPTION_FRT=0; //initial 
  for(i=0;i<8;i++)
  {
    MC32T8132FTtest(mm);
    //�����ж�Ƶ���Ƿ�����Ҫ��
    if (IRC_VALUE == IRC_FreqType)
    {
      break;
    }
    //���²����������� ������У׼
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
    OPTION_FRT=mm;
    return 1;//У׼�ɹ� 
  } 
  OPTION_FRT=0XFF;
  return 0;//У׼ʧ�� 
}

/*******************************************************************************
* Function Name  : MC32T8132_Program
* Description    : MC32T8132�в������ܣ����жϱ��β����ǲ��ǲ���������ǲ�������
*                  ֻ���в�������������д���򣬾Ͳ����в���������ֻ��д
* Input          : None
* Output         : None
* Return         : 1 :�ɹ�  0��ʧ��
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
  u16   erase_num;//Ҫ��
  u16    erase_en;//
  u8    temp;
  u16   temp1;
  u8    i;
  
  u8    id_length=0;
  u16   id_addr0,id_addr1,id_addr2,id_addr3;
  u32   id_end,current_id,max_id;  
  
//------------------------------��ģʽ----------------------------  
  MC32T8132_MODEL_IN(vdd50);
  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID) 
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  }
 
//------------------------------����----------------------------   
  //���жϱ��β�����ֻ����  ����ֻ���  ���ǲ������ٱ��
  //�˴�����λ��ͨѶ�涨 flash��Addr_Flash_OptionProtec+5��ַ��Ҫ�����Ŀ飨�����ֽڣ���flash��Addr_Flash_OptionProtec+7��ַ�� �������ֵ��0��1��
  FM_CS_0;
  FMReadMore(Addr_Flash_OptionProtec+9);//ͨ����ǰһ����ַ �����ַ��1 �����´�Ҫ���ĵ�ַAddr_Flash_OptionProtec+5*2-1
//  FMReadMore(Addr_Flash_OptionProtec+1);
  Fdata =Rxdata;
  SPI_ReadByte();//��flash��ȡһ���ֽ�
  FdataL=Rxdata;
  CheckSum +=FdataL;
  SPI_ReadByte();//�ٴ�flash��ȡһ���ֽ�
  FdataH=Rxdata;
  CheckSum +=FdataH;
  Fdata=FdataH*0x100+FdataL;//���һ����  
  erase_num = Fdata;
  SPI_ReadByte();//��flash��ȡһ���ֽ�
  FdataL=Rxdata;
  CheckSum +=FdataL;
  SPI_ReadByte();//�ٴ�flash��ȡһ���ֽ�
  FdataH=Rxdata;
  CheckSum +=FdataH;
  Fdata=FdataH*0x100+FdataL;//���һ����  
  erase_en = Fdata;
  FM_CS_1;
  
  if(erase_en&0x0100)
  {
    WriteFlag = 1;//������¼
  }
  else
  {
    WriteFlag = 0;//��Ƭ��¼
  }  
  temp = erase_en & 0x0006;
  if(temp == 0x00 || temp == 0x06)//��Ҫ����
  {//��PCͨѶЭ���� ��00-��������̣�01-ֻ��̣�11-ֻ����
    temp = (((u8)erase_en) & 0x01)<<4;//��Ϊ1��0
    if(erase_num == 0x00)//ȫ�� ������δ��ס�Ŀ�
    {
      ReFlag = MC32T8132_ERASE(0x20+temp);//ȫ��Ϊ1��0
      if(ReFlag != 1)//����ʧ��
      {
        ERORR_VALUE = ReFlag;//
        return 0;
      }
    }
    else//���
    {
      for(i=0;i<16;i++)
      {
        temp1 = erase_num&(0x0001<<i);
        if(temp1)//��Ҫ������i��
        {
          ReFlag = MC32T8132_ERASE(0x00+temp+i);//���Ϊ1��0
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
          else//����ʧ��
          {
            ERORR_VALUE = ReFlag;//
            return 0;
          }
        }
      }
    }
    if((erase_en & 0x0006) == 0x06)//ֻ����
    {
      return 1;
    }
  }
  
//------------------------------IRCУ׼----------------------------     
  if (DeviceConfig_xx.IRC_Select==0x01)//��ʽ����
//  if (DeviceConfig_xx.IRC_Select==0x00)//���Դ���
  {
    //ReFlag=MC32T8132IRC(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);   
    ReFlag=MC32T8132IRC_8Bit(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
    ConfigTIM4();//�رն�ʱ��TIMER3 TIMER4
    if(ReFlag == 0) 
    {
      ERORR_VALUE=IRC_Value_false;
      return 0;
    }
  }

//------------------------------��ģʽ----------------------------  
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
    
    //MCU��оƬ�ſ��ܻ��й������������    ����оƬ����Ҫ����iic_data�Ĵ���
    id_length=iic_data[1];
    id_addr0=(iic_data[2]<<8)+iic_data[3];// ADDRL
    id_addr1=(iic_data[4]<<8)+iic_data[5];
    id_addr2=(iic_data[6]<<8)+iic_data[7];
    id_addr3=(iic_data[8]<<8)+iic_data[9];//ADDRH
    current_id=(iic_data[10]<<24)+(iic_data[11]<<16)+(iic_data[12]<<8)+iic_data[13];//��ǰ�Ѿ���д����оƬд��ı��
    id_end=(iic_data[15]<<24)+(iic_data[16]<<16)+(iic_data[17]<<8)+iic_data[18];//��д�ɹ��Ĵ��� ����ֵ
    max_id=(iic_data[27]<<24)+(iic_data[28]<<16)+(iic_data[29]<<8)+iic_data[30]; //max-id//оƬ�����
    if (current_id>max_id)
    {
      iic_data[10]=iic_data[23];
      iic_data[11]=iic_data[24];
      iic_data[12]=iic_data[25];
      iic_data[13]=iic_data[26];
    }
    if (iic_data[0]==0x0a)//��д�������ƹ��ܴ�
    {
      if (OKcounter>=id_end)//��д�ɹ��Ĵ����ﵽ����ֵ
      {
        ERORR_VALUE=OK_Counter_Full;
        return 0; // full limit
      }
    }
    
    
    ROMReadData=MC321_ReadByte(0x0000); //set ADDR
    for(OTP_ADDR=DeviceConfig_xx.RomFirAddr;OTP_ADDR<=DeviceConfig_xx.RomEndAddr;)
    {
      SPI_ReadByte();//��flash��ȡһ���ֽ�
      FdataL=Rxdata;
      CheckSum +=FdataL;
      SPI_ReadByte();//�ٴ�flash��ȡһ���ֽ�
      FdataH=Rxdata;
      CheckSum +=FdataH;
      Fdata=FdataH*0x100+FdataL;//���һ����

      mcu_addr=OTP_ADDR/2;
      OTP_ADDR=OTP_ADDR+2;//�Ѿ����������ֽ����� 
      
      if (iic_data[0]==0x0a) //id enable MCU��оƬ�ſ��ܻ��й������������
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
        if (Fdata!=0xffff)
        {
          while(ForceReCnt--)
          {
            MC321_W(Fdata); //wr data to rom
          }
        }
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
        
        if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//�ڹ涨��ȡУ������´ﵽ��Ҫ��ĳɹ����� ��д�ɹ�
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
 //-------------------------����дOPTION��ش���----------------------- 
  //Write OPTION  
  if(DeviceConfig_xx.WriteEn & 0xf0)
  {
    FM_CS_0;
    FMReadMore(Addr_Flash_OptionProtec-1);
    Fdata =Rxdata;
    
    ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr); //set ADDR
    for(op_addr=0;op_addr<DeviceConfig_xx.OptionSize;op_addr++)//
    {
      SPI_ReadByte();//��flash��ȡһ���ֽ�
      FdataL=Rxdata;
      CheckSum +=FdataL;
      SPI_ReadByte();//�ٴ�flash��ȡһ���ֽ�
      FdataH=Rxdata;
      CheckSum +=FdataH;
      Fdata=FdataH*0x100+FdataL;//���һ����

      //option1 
      if(op_addr==1)
      {
        Fdata = (Fdata&0xff00) + (OSCCAL&0x00ff);       //У׼�õ���Ƶ��У׼ֵ
      }
      //option2                 //���ݲ��Խ�� ��ʽ��Ʒ��Ҫд��                                                
      if(op_addr==2)//�¶�У׼λ ���Խ׶� ������λ��д  ��ʽ�汾дУ׼�õ���ֵ
      {          
        temp1 = TADJ;
        temp1 = temp1<<9;
        Fdata = (Fdata&0x81ff)+temp1;
        
        ROMReadData = MC321_ReadByte(0x800b);
        ROMReadData = ROMReadData&0x001f;//ȡ0x800b��5λ
        Fdata = (Fdata&0xffe0)+ROMReadData;      
        
        ROMReadData=MC321_ReadByte(0x8002); //set ADDR
      }
      //option3 //
      if(op_addr==3)//ȡ�ɲ�д���ֵ 
      {          
        
        ROMReadData = MC321_ReadByte(0x800c);
        ROMReadData = ROMReadData&0x007f;//ȡ0x800b��7λ
        Fdata = (Fdata&0xff80)+ROMReadData;  
        
        ROMReadData = MC321_ReadByte(0x800d);
        ROMReadData = ROMReadData&0x01ff;//ȡ0x800d��9λ
        ROMReadData = ROMReadData<<7;
        Fdata = (Fdata&0x007f)+ROMReadData;   
        
        ROMReadData=MC321_ReadByte(0x8003); //set ADDR
      }
      
      ReCnt=0;
      while(++ReCnt<6)  
      {	
        if (Fdata == 0xffff)  // 
        {
          ROMReadData=MC321_R();
          if(ROMReadData == 0xffff)
          {//����Ҫд
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
      //�����ж��Ƿ�д��
      if (op_addr<DeviceConfig_xx.OptionSize-1)
      {//û��д�� rom��ַ��1 ����д
        MC301_Instruct(AddrINC);
      }
    }
    FM_CS_1;
  }
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  return 1;
}