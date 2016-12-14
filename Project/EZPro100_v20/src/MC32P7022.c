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
#include "MC32P7022.h"
#include "MC9033.h"


void MC32P7022FTtest_11Bit(u16 irc_adj_value,u8 TempADJ)
{
  u16 temp=0x0400;
  u8  adjH,adjL;
  u16 i;
  u16 cnt[3];
  u16   Fdata;
  //OPTION 1��
  //bit15:11 --  bit5:bit0 TADJ
  //bit10:0  --  bit10:0   OSCCAL
  
  Fdata = ((TempADJ & 0x001f)<<11) + (irc_adj_value & 0x07ff);
  adjH=(Fdata&0xff00)>>8; //H
  adjL=Fdata&0x00ff; //L  
  
  temp=0x3c00+adjL;
  MC321_IRC_INST(temp);
  MC321_IRC_INST(0x57fb); //option1 L
  MC321_IRC_INST(0x0000);

  temp=0x3c00+adjH;
  MC321_IRC_INST(temp); //
  MC321_IRC_INST(0x57fc); //option1 H  
  
  MC321_IRC_INST(0x0000);
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


//������IRC���԰�
/*
u8 MC32P7022IRC_11Bit(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i;
  u8 optL,optH,opt2L,Fdata_H;
  u16 mm,Fdata,temp0_FT,temp1_FT;
  u8 OPTION_FAS;
  u8 OPTION_TEMPADJ_1,OPTION_TEMPADJ_2;
  
  FM_CS_0;
  mm=0x400 ;//& (rdv<<8);
  //power on & model in
  MC321_MODEL_IN(vdd30);
  Delay_1ms(1);
  MC301_Instruct(TestMod);
  for(i=0;i<5;i++)
  {
    MC321_IRC_INST(0X0000); //5 NOP
  }  
  //���ľ�Ҫ�����������ָ����
  MC321_IRC_INST(0x3c03);
  MC321_IRC_INST(0x57ae);     //дopt2 H
  
  if (FMReadOne(Addr_Flash_OptionProtec+7)==1)//ȡflash�� option 2 ���ֽ�
  {
    optH =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }    

  mm=0x3c00+optH;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fe);     //дopt2 H
  
  OPTION_FAS = optH & 0x07; //ȷ����16M 8M 4M 2M 1M 455K��һ��

  if(OPTION_FAS == 0)//16M
  {
//    OPTION_TEMPADJ_1 = 0x0a;
//    OPTION_TEMPADJ_2 = 0x09;
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;
    optL = 0x70;//opt0 L
//    optL = 0x00;//opt0 L
  }
  else if(OPTION_FAS == 1)//8M
  {
//    OPTION_TEMPADJ_1 = 0x12;
//    OPTION_TEMPADJ_2 = 0x11;
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;   
    optL = 0x60;//opt0 L
  }
  else if(OPTION_FAS == 2)//4M
  {
//    OPTION_TEMPADJ_1 = 0x29;
//    OPTION_TEMPADJ_2 = 0x2a;
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
    optL = 0x50;//opt0 L
  }
  else if(OPTION_FAS == 3)//2M
  {
//    OPTION_TEMPADJ_1 = 0x3F;
//    OPTION_TEMPADJ_2 = 0x3E;  
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
    optL = 0x40;//opt0 L
  }   
  else if(OPTION_FAS == 4)//1M
  {
//    OPTION_TEMPADJ_1 = 0x3F;
//    OPTION_TEMPADJ_2 = 0x3E;  
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
    optL = 0x30;//opt0 L
  }   
  else if(OPTION_FAS == 5)//455K
  {
//    OPTION_TEMPADJ_1 = 0x3F;
//    OPTION_TEMPADJ_2 = 0x3E;  
    IRC_FreqType = 2275;
    IRC_FreqMin = 2268;
    IRC_FreqMax = 2282;    
    optL = 0x10;//opt0 L
  }   
  else//��λ��û������OPTION 
  {
      return 0;
  }
  mm=0x3c00+optL;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57f9);     //дopt0 L
  
  if (FMReadOne(Addr_Flash_OptionProtec+6)==1)//ȡflash�� option 2 ���ֽ�
  {
    optL =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }  
  mm=0x3c00+optL;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fd);     //дopt2 L
  
  if (FMReadOne(Addr_Flash_OptionProtec+5)==1)//ȡoption 1 H
  {
    optH =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }
  mm=0x3c00+optH;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fc);     //дopt1 H
  
  optL&=0x01;
  optL<<=5;
  optH&=0xf8;
  optH>>=3;
  OPTION_TEMPADJ_1 = optL|optH;//��λ�������¶�ֵ
  
  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;

  MC321_IRC_INST(0x3c01);     //movai 0x01
  MC321_IRC_INST(0x57f8);     //movra 0x1f8 //set HIRC output to SDO

  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;   

//  Delay_1ms(250);//���Է��ֱ���Ҫ��ʱ > 212
  mm=0x400;//Ƶ��У׼λ�ȸ��м�ֵ
  OPTION_FRT=0; //initial 
  OPTION_FRT2=0; 
  IRC_MODEL=0;
  for(i=0;i<11;i++)
  {
    MC32P7022FTtest_11Bit(mm,OPTION_TEMPADJ_1);
    if (IRC_VALUE == IRC_FreqType)
    {
      break;
    }
    if (IRC_VALUE<IRC_FreqType)
    {
      mm |=0x400 >>i;
    } 
    else
    {
      mm &=~(0x400>>i);
    }
    mm |=0x200>>i;
  }
  MC32P7022FTtest_11Bit(mm,OPTION_TEMPADJ_1);
  temp1_FT=IRC_VALUE;
  MC32P7022FTtest_11Bit(mm+1,OPTION_TEMPADJ_1);
  temp0_FT=IRC_VALUE;
  if((temp0_FT-IRC_FreqType)<(IRC_FreqType-temp1_FT))
  {
    mm=mm+1;
    IRC_VALUE=temp0_FT;
  }
  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    OPTION_FRT=mm/256;
    OPTION_FRT2=mm%256;
    OPTION_TEMPADJ = OPTION_TEMPADJ_1;
    return 1;//��һ��У��ɹ�������У��
  }

  OPTION_FRT=0xff;
  OPTION_FRT2=0xff;
  OPTION_TEMPADJ = 0xff;  
  return 0;
  //}У�����
}
*/

//������IRCУ����ʽ��
u8 MC32P7022IRC_11Bit(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i,j;
  u8 optL,optH;
  u16 mm,temp,temp0_FT,temp1_FT;
  u8 OPTION_FAS;
  u8 OPTION_TEMPADJ_1,OPTION_TEMPADJ_2,OPTION_TEMPADJ_3;
  
  FM_CS_0;
  mm=0x400 ;//& (rdv<<8);
  //power on & model in
//  MC321_MODEL_IN(vdd30);
  Delay_1ms(1);
  MC301_Instruct(TestMod);
  for(i=0;i<5;i++)
  {
    MC321_IRC_INST(0X0000); //5 NOP
  }  
  //���ľ�Ҫ�����������ָ����
  MC321_IRC_INST(0x3c03);
  MC321_IRC_INST(0x57ae);     //
  
  if (FMReadOne(Addr_Flash_OptionProtec+7)==1)//ȡflash�� option 2 ���ֽ�
  {
    optH =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }    

  mm=0x3c00+optH;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fe);     //дopt2 H
  
  OPTION_FAS = optH & 0x07; //ȷ����16M 8M 4M 2M 1M 455K��һ��

  if(OPTION_FAS == 0)//16M
  {
    OPTION_TEMPADJ_1 = 0x0a;
    OPTION_TEMPADJ_2 = 0x09;
    OPTION_TEMPADJ_3 = 0x0b;
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;
    optL = 0x70;//opt0 L
//    optL = 0x00;//opt0 L
  }
  else if(OPTION_FAS == 1)//8M
  {
    OPTION_TEMPADJ_1 = 0x12;
    OPTION_TEMPADJ_2 = 0x11;
    OPTION_TEMPADJ_3 = 0x13;
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;   
    optL = 0x60;//opt0 L
  }
  else if(OPTION_FAS == 2)//4M
  {
    OPTION_TEMPADJ_1 = 0x29;
    OPTION_TEMPADJ_2 = 0x2a;
    OPTION_TEMPADJ_3 = 0x28;
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
    optL = 0x50;//opt0 L
  }
  else if(OPTION_FAS == 3)//2M
  {
    OPTION_TEMPADJ_1 = 0x3F;
    OPTION_TEMPADJ_2 = 0x3E;
    OPTION_TEMPADJ_3 = 0x3D;
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
    optL = 0x40;//opt0 L
  }   
  else if(OPTION_FAS == 4)//1M
  {
    OPTION_TEMPADJ_1 = 0x3F;
    OPTION_TEMPADJ_2 = 0x3E; 
    OPTION_TEMPADJ_3 = 0x3D;
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
    optL = 0x30;//opt0 L
  }   
  else if(OPTION_FAS == 5)//455K
  {
    OPTION_TEMPADJ_1 = 0x3F;
    OPTION_TEMPADJ_2 = 0x3E;
    OPTION_TEMPADJ_3 = 0x3D;
    IRC_FreqType = 2275;
    IRC_FreqMin = 2268;
    IRC_FreqMax = 2282;    
    optL = 0x10;//opt0 L
  }   
  else//��λ��û������OPTION 
  {
      return 0;
  }
  mm=0x3c00+optL;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57f9);     //дopt0 L
  
  if (FMReadOne(Addr_Flash_OptionProtec+6)==1)//ȡflash�� option 2 ���ֽ�
  {
    optL =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }  
  mm=0x3c00+optL;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fd);     //дopt2 L
  
  if (FMReadOne(Addr_Flash_OptionProtec+5)==1)//ȡoption 1 H
  {
    optH =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }
  mm=0x3c00+optH;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fc);     //дopt1 H
  
  /*
  optL&=0x01;
  optL<<=5;
  optH&=0xf8;
  optH>>=3;
  OPTION_TEMPADJ_1 = optL|optH;//��λ�������¶�ֵ
  */
  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;

  MC321_IRC_INST(0x3c01);     //movai 0x01
  MC321_IRC_INST(0x57f8);     //movra 0x1f8 //set HIRC output to SDO

  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;   

//  Delay_1ms(250);//���Է��ֱ���Ҫ��ʱ > 212 �������Ѿ���� 
  for(j=0;j<3;j++)
  {
    //�������¶�У׼ֵ���λ
    if(j==0)
    {
      OPTION_TEMPADJ = OPTION_TEMPADJ_1;          
    }
    else if(j==1)
    {
      OPTION_TEMPADJ = OPTION_TEMPADJ_2;
    }
    else if(j==2)
    {
      OPTION_TEMPADJ = OPTION_TEMPADJ_3;
    }
    optL = (OPTION_TEMPADJ&0x20)>>5;
    if(FMReadOne(Addr_Flash_OptionProtec+6)==1)//ȡflash�� option 2 ���ֽ�
    {
      optL = (Rxdata&0xfe)|optL;
    }
    else
    {
      ERORR_VALUE=FM_Read_false;
      FM_CS_1;
      return 0;
    }    
    temp=0x3c00+optL;
    MC321_IRC_INST(temp); //
    MC321_IRC_INST(0x57fd); //option2 L
    
    
    
    mm=0x400;//Ƶ��У׼λ�ȸ��м�ֵ
    OPTION_FRT=0; //initial 
    OPTION_FRT2=0; 
    IRC_MODEL=0;
    for(i=0;i<11;i++)
    {
      MC32P7022FTtest_11Bit(mm,OPTION_TEMPADJ);
      if (IRC_VALUE == IRC_FreqType)
      {
        break;
      }
      if (IRC_VALUE<IRC_FreqType)
      {
        mm |=0x400 >>i;
      } 
      else
      {
        mm &=~(0x400>>i);
      }
      mm |=0x200>>i;
    }
    MC32P7022FTtest_11Bit(mm,OPTION_TEMPADJ);
    temp1_FT=IRC_VALUE;
    MC32P7022FTtest_11Bit(mm+1,OPTION_TEMPADJ);
    temp0_FT=IRC_VALUE;
    if((temp0_FT-IRC_FreqType)<(IRC_FreqType-temp1_FT))
    {
      mm=mm+1;
      IRC_VALUE=temp0_FT;
    }
    if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
    {
      OPTION_FRT=mm/256;
      OPTION_FRT2=mm%256;
      OPTION_TEMPADJ = OPTION_TEMPADJ;
      return 1;//У��ɹ�������У��
    }  
  }

  OPTION_FRT=0xff;//У��ʧ��
  OPTION_FRT2=0xff;
  OPTION_TEMPADJ = 0xff;  
  return 0;
  //}У�����
}

u8 MC32P7022IRC_11Bit_Check(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 optL,optH;
  u16 mm,temp;
  u8 OPTION_FAS;
  
  FM_CS_0;
  
  if (FMReadOne(Addr_Flash_OptionProtec+7)==1)//ȡflash�� option 2 ���ֽ�
  {
    optH =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }    
  
  OPTION_FAS = optH & 0x07; //ȷ����16M 8M 4M 2M 1M 455K��һ��
  if(OPTION_FAS == 0)//16M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;
  }
  else if(OPTION_FAS == 1)//8M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;   
  }
  else if(OPTION_FAS == 2)//4M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
  }
  else if(OPTION_FAS == 3)//2M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
  }   
  else if(OPTION_FAS == 4)//1M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
  }   
  else if(OPTION_FAS == 5)//455K
  {
    IRC_FreqType = 2275;
    IRC_FreqMin = 2268;
    IRC_FreqMax = 2282;    
  }   
  else//��λ��û������OPTION 
  {
      return 0;
  }
  
  optL = (OPTION_TEMPADJ&0x20)>>5;
  if(FMReadOne(Addr_Flash_OptionProtec+6)==1)//ȡflash�� option 2 ���ֽ�
  {
    optL = (Rxdata&0xfe)|optL;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }    
  temp=0x3c00+optL;
  MC321_IRC_INST(temp); //
  MC321_IRC_INST(0x57fd); //option2 L
  
  mm=OPTION_FRT*256+OPTION_FRT2;
  MC32P7022FTtest_11Bit(mm,OPTION_TEMPADJ);

  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    return 1;//У��ɹ�������У��
  }  

  OPTION_FRT=0xff;//У��ʧ��
  OPTION_FRT2=0xff;
  OPTION_TEMPADJ = 0xff;  
  return 0;
  //}У�����
}

u8 MC32P7022IRC_11Bit_ModeInCheck(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i;
  u8 optL,optH;
  u16 mm,temp;
  u8 OPTION_FAS;
  
  FM_CS_0;

  Delay_1ms(1);
  MC301_Instruct(TestMod);
  for(i=0;i<5;i++)
  {
    MC321_IRC_INST(0X0000); //5 NOP
  }  
  //���ľ�Ҫ�����������ָ����
  MC321_IRC_INST(0x3c03);
  MC321_IRC_INST(0x57ae);     //
  
  if (FMReadOne(Addr_Flash_OptionProtec+7)==1)//ȡflash�� option 2 ���ֽ�
  {
    optH =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }    

  mm=0x3c00+optH;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fe);     //дopt2 H
  
  OPTION_FAS = optH & 0x07; //ȷ����16M 8M 4M 2M 1M 455K��һ��

  if(OPTION_FAS == 0)//16M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;
    optL = 0x70;//opt0 L
//    optL = 0x00;//opt0 L
  }
  else if(OPTION_FAS == 1)//8M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;   
    optL = 0x60;//opt0 L
  }
  else if(OPTION_FAS == 2)//4M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
    optL = 0x50;//opt0 L
  }
  else if(OPTION_FAS == 3)//2M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
    optL = 0x40;//opt0 L
  }   
  else if(OPTION_FAS == 4)//1M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
    optL = 0x30;//opt0 L
  }   
  else if(OPTION_FAS == 5)//455K
  {
    IRC_FreqType = 2275;
    IRC_FreqMin = 2268;
    IRC_FreqMax = 2282;    
    optL = 0x10;//opt0 L
  }   
  else//��λ��û������OPTION 
  {
      return 0;
  }
  mm=0x3c00+optL;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57f9);     //дopt0 L
  
  if (FMReadOne(Addr_Flash_OptionProtec+6)==1)//ȡflash�� option 2 ���ֽ�
  {
    optL =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }  
  mm=0x3c00+optL;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fd);     //дopt2 L
  
  if (FMReadOne(Addr_Flash_OptionProtec+5)==1)//ȡoption 1 H
  {
    optH =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }
  mm=0x3c00+optH;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fc);     //дopt1 H
  
  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;

  MC321_IRC_INST(0x3c01);     //movai 0x01
  MC321_IRC_INST(0x57f8);     //movra 0x1f8 //set HIRC output to SDO

  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;   
 

  optL = (OPTION_TEMPADJ&0x20)>>5;
  if(FMReadOne(Addr_Flash_OptionProtec+6)==1)//ȡflash�� option 2 ���ֽ�
  {
    optL = (Rxdata&0xfe)|optL;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }    
  temp=0x3c00+optL;
  MC321_IRC_INST(temp); //
  MC321_IRC_INST(0x57fd); //option2 L

  mm=OPTION_FRT*256+OPTION_FRT2;
  MC32P7022FTtest_11Bit(mm,OPTION_TEMPADJ);
      
  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    //OPTION_FRT=mm/256;
    //OPTION_FRT2=mm%256;
    //OPTION_TEMPADJ = OPTION_TEMPADJ;
    return 1;//У��ɹ�������У��
  }  
  
  OPTION_FRT=0xff;//У��ʧ��
  OPTION_FRT2=0xff;
  OPTION_TEMPADJ = 0xff;  
  return 0;
  //}У�����
}

/*******************************************************************************
* Function Name  : MC32T8132_Program
* Description    : MC32T8132�в������ܣ����жϱ��β����ǲ��ǲ���������ǲ�������
*                  ֻ���в�������������д���򣬾Ͳ����в���������ֻ��д
* Input          : None
* Output         : None
* Return         : 1 :�ɹ�  0��ʧ��
*******************************************************************************/
//u8 MC32P7022_Program()
//{
//  u8    ReCnt=0,opt_shift;
//  u16   Fdata,ROMReadData;
//  u8	FdataL,FdataH;  
//  u8    ReFlag=0;
//  u16   CheckSum=0;
//  u16   mcu_addr;   
//  u32   op_addr;
//  u8    i,temp1,flag_oncewrite=0;
//  u16   temp;
//  
//  u8    id_length=0;
//  u16   id_addr0,id_addr1,id_addr2,id_addr3;
//  u32   id_end,current_id,max_id;  
//  
//  u8    j;
//  u8    ForceReCnt=0;  
//  u8    SuccessCnt;  
//  u8    option_shift=0;  
//  u16   Addres;
// 
//  WriteConfig_xx.WriteCheckNeed=0;
//  WriteConfig_xx.ReadCnt=1;
//  WriteConfig_xx.ReadOkCnt=1;
//  WriteConfig_xx.ReWriteCnt=3;
//  WriteConfig_xx.ForceWriteCnt=1;
//  WriteConfig_xx.AllCheckNeed=2;   
////------------------------------�Ƚ�ģʽ----------------------------
//  VDD30V_Off;
//  POWER_OFF(vpp00,vdd00);
//  Delay_1ms(1);  
//  MC321_MODEL_IN(vdd30);  
//  Delay_1ms(1);
//  CLN_VDD2GND_Off;
//  VPP12V_On;
//  Delay_1ms(10);
//  Fdata=MC321_ReadByte(0XFFFF);
//  if (Fdata !=DeviceConfig_xx.MCU_ID)
//  {
//    ERORR_VALUE=OTP_ModeIn_false;
//    return 0;
//  }
// 
////------------------------------IRCУ׼----------------------------
//  if (DeviceConfig_xx.IRC_Select==0x01)//��ʽ����
////  if (DeviceConfig_xx.IRC_Select==0x00)//���Դ���
//  {
//    ReFlag=MC32P7022IRC_11Bit(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
//    if(ReFlag==1)
//    {
//      ReFlag=MC32P7022IRC_11Bit_Check(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
//    }
//    ConfigTIM4();
//    if(ReFlag == 0) 
//    {
//      ERORR_VALUE=IRC_Value_false;
//      return 0;
//    }
//  }
//  
////------------------------------�ٽ�ģʽ----------------------------  
//  VDD30V_Off;
//  POWER_OFF(vpp00,vdd00);
//  Delay_1ms(1);    
//  MC321_MODEL_IN(vdd30);  
//  Delay_1ms(1);
//  CLN_VDD2GND_Off;
//  VPP12V_On;
//  Delay_1ms(10);  
//  Fdata=MC321_ReadByte(0XFFFF);
//  if (Fdata !=DeviceConfig_xx.MCU_ID) 
//  {
//    ERORR_VALUE=OTP_ModeIn_false;
//    return 0;
//  }
//
////------------------------------Write Page---------------------------- 
//  FM_CS_0;
//  if (FMReadOne(Addr_Flash_OptionProtec)==1)//
//  {
//    FdataL =Rxdata;
//  }
//  else
//  {
//    ERORR_VALUE=FM_Read_false;
//    FM_CS_1;
//    return 0;
//  }
//  if (FMReadOne(Addr_Flash_OptionProtec+1)==1)//
//  {
//    FdataH =Rxdata;
//  }
//  else
//  {
//    ERORR_VALUE=FM_Read_false;
//    FM_CS_1;
//    return 0;
//  }    
//  Fdata=FdataH*0x100+FdataL;//���һ����  
//  WriteConfig_xx.OptionValue[0]=Fdata;
//  WriteConfig_xx.OptionAddr[0]=0x8000;
//  ReCnt=WriteConfig_xx.ReWriteCnt;
//  while(ReCnt--) 
//  {	
//    ForceReCnt=WriteConfig_xx.ForceWriteCnt;
//    if (Fdata != 0xffff)  // 
//    {
//      while(ForceReCnt--)
//      {
//        MC321_WriteByte(0x8000,Fdata);
//      }
//    }    
//    if (WriteConfig_xx.AllCheckNeed == 2)
//    {
//      for(i=1;i<3;i++)
//      {
//        SuccessCnt = 0;
//        for(j=0;j<WriteConfig_xx.ReadCnt;j++)
//        {
//          ROMReadData=MC321_ReadByte_DiffMode(i,0x8000);
//          if (ROMReadData==Fdata)
//          {
//            SuccessCnt++;
//          }
//          else
//          {
//          }
//          if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
//          {
//            break;
//          }
//        } 
//        if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//�ڹ涨��ȡУ������´ﵽ��Ҫ��ĳɹ����� ��д�ɹ�
//        {
//        }
//        else
//        {
//          if(i==1)
//          {
//            ERORR_VALUE=OPTION_MAGRIN1_ERROR;
//          }
//          else if(i==2)//������
//          {
//            ERORR_VALUE=OPTION_OFF_MAGRIN_ERROR;
//          }
//          break;
//        }
//      }
//      if (i==3)//3��ģʽ��ȡ����д�������һ�� ��д�ɹ� 2
//      {
//        break;
//      }
//      else
//      {
//        OTP_ADDR= 0x8000;
//        return 0;
//      } 
//    }  
//    else if(WriteConfig_xx.AllCheckNeed == 1)
//    {
//      SuccessCnt = 0;
//      for(j=0;j<WriteConfig_xx.ReadCnt;j++)
//      {
//        ROMReadData=MC321_ReadByte(0x8000);
//        if (ROMReadData==Fdata)
//        {
//          SuccessCnt++;
//        }
//        else
//        {
//        }
//        if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
//        {
//          break;
//        }
//      } 
//      if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//�ڹ涨��ȡУ������´ﵽ��Ҫ��ĳɹ����� ��д�ɹ�
//      {
//      }
//      else
//      {
//        ERORR_VALUE=OPTION_NOMALL_ERROR;
//        OTP_ADDR= 0x8000;
//        return 0;
//      }
//    }
//  }
//
//  if((Fdata&0x0070) == 0x0030)//1����дģʽ
//  {
//    DeviceConfig_xx.RomEndAddr = 0x1fbf;
//    opt_shift = 0;
//  }
//  else if((Fdata&0x0070) == 0x0050)//2����дģʽ
//  {
//    DeviceConfig_xx.RomEndAddr = 0xfbf;
//    if((Fdata&0x000c)==0x0004)//��1ҳ
//    {
//      opt_shift = 0;
//    }
//    if((Fdata&0x000c)==0x0000)//��2ҳ
//    {
//      opt_shift = 5;
//    }    
//  }
//  else if((Fdata&0x0070) == 0x0060)//4����дģʽ
//  {
//    DeviceConfig_xx.RomEndAddr = 0x7bf;
//    if((Fdata&0x000f)==0x0007)//��1ҳ
//    {
//      opt_shift = 0;
//    }
//    else if((Fdata&0x000f)==0x0003)//��2ҳ
//    {
//      opt_shift = 5;
//    }    
//    else if((Fdata&0x000f)==0x0001)//��3ҳ
//    {
//      opt_shift = 10;
//    }  
//    else if((Fdata&0x000f)==0x0000)//��4ҳ
//    {
//      opt_shift = 15;
//    }  
//  }
//  FM_CS_1;  
//
////------------------------------�ٽ�ģʽ---------------------------- 
//  VDD30V_Off;
//  POWER_OFF(vpp00,vdd00);
//  Delay_1ms(1);    
//  MC321_MODEL_IN(vdd30);  
//  Delay_1ms(1);
//  CLN_VDD2GND_Off;
//  VPP12V_On;
//  Delay_1ms(10);  
//  Fdata=MC321_ReadByte(0XFFFF);
//  if (Fdata !=DeviceConfig_xx.MCU_ID) 
//  {
//    ERORR_VALUE=OTP_ModeIn_false;
//    return 0;
//  }
//
////------------------------------Write ROM---------------------------- 
//  if (DeviceConfig_xx.WriteEn & 0x0f)
//  {
//    CheckSum=0;
//    FM_CS_0;
//    FMReadMore(Addr_Flash_ROMStart+DeviceConfig_xx.RomFirAddr-1);
//    Fdata =Rxdata;
//
//    //MCU��оƬ�ſ��ܻ��й������������    ����оƬ����Ҫ����iic_data�Ĵ���
//    id_length=iic_data[1];
//    id_addr0=(iic_data[2]<<8)+iic_data[3];// ADDRL
//    id_addr1=(iic_data[4]<<8)+iic_data[5];
//    id_addr2=(iic_data[6]<<8)+iic_data[7];
//    id_addr3=(iic_data[8]<<8)+iic_data[9];//ADDRH
//    current_id=(iic_data[10]<<24)+(iic_data[11]<<16)+(iic_data[12]<<8)+iic_data[13];//��ǰ�Ѿ���д����оƬд��ı��
//    id_end=(iic_data[15]<<24)+(iic_data[16]<<16)+(iic_data[17]<<8)+iic_data[18];//��д�ɹ��Ĵ��� ����ֵ
//    max_id=(iic_data[27]<<24)+(iic_data[28]<<16)+(iic_data[29]<<8)+iic_data[30]; //max-id//оƬ�����
//    if (current_id>max_id)
//    {
//      iic_data[10]=iic_data[23];
//      iic_data[11]=iic_data[24];
//      iic_data[12]=iic_data[25];
//      iic_data[13]=iic_data[26];
//    }
//    if (iic_data[0]==0x0a)//��д�������ƹ��ܴ�
//    {
//      if (OKcounter>=id_end)//��д�ɹ��Ĵ����ﵽ����ֵ
//      {
//        ERORR_VALUE=OK_Counter_Full;
//        return 0; // full limit
//      }
//    }
//    
//    ROMReadData=MC321_ReadByte(0x0000); //set ADDR
//    for(OTP_ADDR=DeviceConfig_xx.RomFirAddr;OTP_ADDR<=DeviceConfig_xx.RomEndAddr;)
//    {
//      SPI_ReadByte();
//      FdataL=Rxdata;
//      CheckSum +=FdataL;
////      CheckSum_c += FdataL;
//      SPI_ReadByte();
//      FdataH=Rxdata;
//      CheckSum +=FdataH;
////      CheckSum_c += (FdataH<<8);
//      Fdata=FdataH*0x100+FdataL;
//      //Fdata=Fdata & 0x3fff;
//      //2012.09.12 dynamic id
//      mcu_addr=OTP_ADDR/2;
//      OTP_ADDR=OTP_ADDR+2;						
//      if (iic_data[0]==0x0a) //id enable MCU��оƬ�ſ��ܻ��й������������
//      {
//        if (mcu_addr==id_addr0)
//        {
//          //Fdata=iic_data[13];
//          Fdata=FdataH*0x100+iic_data[13]; // must keep the H byte for code
//        }
//        else if ((mcu_addr==id_addr1)&& (id_length>1))
//        {
//          //Fdata=iic_data[12];
//          Fdata=FdataH*0x100+iic_data[12];
//        }
//        else if ((mcu_addr==id_addr2)&& (id_length>2))
//        {
//          //Fdata=iic_data[11];
//          Fdata=FdataH*0x100+iic_data[11];
//        }
//        else if ((mcu_addr==id_addr3)&&(id_length>3))
//        {
//          //Fdata=iic_data[10];
//          Fdata=FdataH*0x100+iic_data[10];
//        }
//      }
//      //------------------------------------------------
//      Rxdata=Fdata;
//      
//      ReCnt=WriteConfig_xx.ReWriteCnt;
//      while(ReCnt--)  //start wr rom
//      {	 
//        ForceReCnt=WriteConfig_xx.ForceWriteCnt;
//        if (Fdata!=0xffff)
//        {
//          while(ForceReCnt--)
//          {
//            MC321_W(Fdata); //wr data to rom
//          }
//        }
//        SuccessCnt = 0;
//        if(WriteConfig_xx.WriteCheckNeed)
//        {
//          for(i=0;i<WriteConfig_xx.ReadCnt;i++)
//          {
//            ROMReadData=MC321_R();
//            if (ROMReadData==Fdata)
//            {
//              SuccessCnt++;
//            }
//            if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
//            {
//              break;
//            }
//          } 
//        }
//        else
//        {
//          SuccessCnt = WriteConfig_xx.ReadOkCnt;
//        }
//        
//        if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//�ڹ涨��ȡУ������´ﵽ��Ҫ��ĳɹ����� ��д�ɹ�
//        {
//          //ReCnt=0;
//          break;
//        }
//        else
//        {
//          ERORR_VALUE=ROM_Write_false;
//        }
//        
//        if (ReCnt==0)
//        {
//          OTP_ADDR= mcu_addr;
//          return 0;
//        }                    
//      }
//      if (OTP_ADDR<DeviceConfig_xx.RomEndAddr)
//      {
//        MC301_Instruct(AddrINC);
//        if (DeviceConfig_xx.MCU_ID==0x7022)
//        {
//          mcu_addr = MC321_PreRead(PAddrRe);
//          if (mcu_addr!=OTP_ADDR/2)
//          {
//            ERORR_VALUE=ADDR_EER;
//            return 0;
//          }
//        }
//      }      
//    }
//    FM_CS_1;
//  }  
//
////------------------------------Write OPTION ---------------------------- 
//  if(DeviceConfig_xx.WriteEn & 0xf0)
//  {
//    FM_CS_0;
//    FMReadMore(Addr_Flash_OptionProtec+1);
//    Fdata =Rxdata;
//    
//    ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr); //set ADDR
//    for(op_addr=0;op_addr<DeviceConfig_xx.OptionSize;op_addr++)//
//    {
//      SPI_ReadByte();//��flash��ȡһ���ֽ�
//      FdataL=Rxdata;
//      //CheckSum +=FdataL;
//      SPI_ReadByte();//�ٴ�flash��ȡһ���ֽ�
//      FdataH=Rxdata;
//      //CheckSum +=FdataH;
//      Fdata=FdataH*0x100+FdataL;//���һ����
//      
//      //option0 
//      if(op_addr==0)
//      {          
//        Fdata|=0x8000;//��д����λ
//      }      
//      //option1 
//      if(op_addr==1)        
//      {
//        temp = OPTION_FRT*256+OPTION_FRT2;
//        /*���԰�
//        Fdata = (Fdata&0xf800) + temp;       //У׼�õ���Ƶ��У׼ֵ
//        ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+opt_shift);
//        if(ROMReadData!=0xffff)
//        {
//          Fdata=ROMReadData;
//        }    
//        */
//        //��ʽ��
//        Fdata = (OPTION_TEMPADJ<<3)*256+temp;
//        ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+opt_shift);
//        if(ROMReadData!=0xffff)//У�鲢д��
//        {
//          Fdata=ROMReadData;//��������д��ȥ
//          flag_oncewrite = 1;//оƬƵ��У׼ֵ��д��
//        }    
//      }
//      //option2                       
//      if(op_addr==2)//�¶�У׼λ���λ ���Խ׶� ������λ��д  ��ʽ�汾дУ׼�õ���ֵ
//      {                  
//        ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+opt_shift-1);
//        if(flag_oncewrite)//оƬƵ��У׼ֵ��д��
//        {
//          ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+opt_shift);
//          Fdata=(Fdata&0xfffe)|(ROMReadData&0x0001);
//        }    
//        else
//        {
//          temp1 = OPTION_TEMPADJ;
//          temp1 = temp1>>5;
//          Fdata = (Fdata&0xfffe)+temp1;       
//        }      
//      }
//
//      if(op_addr==3)//
//      {          
//        ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+opt_shift);
//        if(ROMReadData==0xffff)//ûд��Vref
//        {
//          ROMReadData = MC321_ReadByte(0x8018);
//          if(ROMReadData==0xffff)//û�гɲ�ֵ
//          {
//            ROMReadData = MC321_ReadByte(0x8015);
//            if(ROMReadData==0xffff)//û���в�ֵ
//            {
//              ERORR_VALUE=OPTION_Write_false;//ûд��adc referenceƵ��У׼ֵ Ҳû�гɲ�adc referenceֵ Ҳû���в�adc referenceֵ ����
//              return 0;
//            }
//          }
//          Fdata = ROMReadData&0x00ff;
//          
//          ROMReadData = MC321_ReadByte(0x8019);
//          if(ROMReadData==0xffff)//û�гɲ�ֵ
//          {
//            ROMReadData = MC321_ReadByte(0x8016);
//            if(ROMReadData==0xffff)//û���в�ֵ
//            {
//              ERORR_VALUE=OPTION_Write_false;//ûд��adc referenceƵ��У׼ֵ Ҳû�гɲ�adc referenceֵ Ҳû���в�adc referenceֵ ����
//              return 0;
//            }
//          } 
//          ROMReadData = ROMReadData<<8;
//          Fdata = Fdata+ROMReadData;            
//        }
//        else
//        {
//          Fdata=ROMReadData;
//        }
//      }
//
//      if(op_addr==4)//
//      {          
//        ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+opt_shift);
//        if(ROMReadData==0xffff)//ûд��Vref
//        {
//          ROMReadData = MC321_ReadByte(0x801A);
//          if(ROMReadData==0xffff)//û�гɲ�ֵ
//          {
//            ROMReadData = MC321_ReadByte(0x8017);
//            if(ROMReadData==0xffff)//û���в�ֵ
//            {
//              ERORR_VALUE=OPTION_Write_false;//ûд��adc referenceƵ��У׼ֵ Ҳû�гɲ�adc referenceֵ Ҳû���в�adc referenceֵ ����
//              return 0;
//            }
//          }
//          ROMReadData = ROMReadData&0x00ff;
//          ROMReadData = ROMReadData<<8;
//          Fdata = (Fdata&0x00ff)+ROMReadData;             
//        }
//        else
//        {
//          Fdata=ROMReadData;
//        }
//      }       
//      WriteConfig_xx.OptionValue[op_addr+1] = Fdata;//��option֮ǰ�Ȱ�ֵ���������Է������ȫ��У��ʱ��ֵ
//      WriteConfig_xx.OptionAddr[op_addr+1]=DeviceConfig_xx.OptionAddr+op_addr+option_shift;
//      ReCnt=WriteConfig_xx.ReWriteCnt;
//      while(ReCnt--)  
//      {	
//        ForceReCnt=WriteConfig_xx.ForceWriteCnt;
//        if (Fdata != 0xffff)  // 
//        {
//          while(ForceReCnt--)
//          {
//            MC321_WriteByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift,Fdata);
//          }
//        }        
//	SuccessCnt = 0;
//	if (WriteConfig_xx.WriteCheckNeed)
//	{
//	  for(i=0;i<WriteConfig_xx.ReadCnt;i++)
//	  {
//	    ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);         
//	    if (ROMReadData==Fdata)
//	    {
//	      SuccessCnt++;
//	    }
//	    else
//	    {
//	    }
//	    if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
//	    {
//	      break;
//	    }
//	  }
//	}
//	else
//	{
//	  SuccessCnt = WriteConfig_xx.ReadOkCnt;
//	}
//        if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//�ڹ涨��ȡУ������´ﵽ��Ҫ��ĳɹ����� ��д�ɹ�
//        {
//          //ReCnt=0;
//          break;
//        }
//	else
//	{
//	  ERORR_VALUE=OPTION_NOMALL_ERROR;
//	}
//	
//        if (ReCnt==0)
//        {        
//          OTP_ADDR=DeviceConfig_xx.OptionAddr+op_addr+option_shift;
//          return 0;
//        }                     
//      }
//    }
//    FM_CS_1;
//  }  
//  
////------------------------------write checksum---------------------------- 
//  ReCnt=0;  
//  while(++ReCnt<3)
//  {
//    if(opt_shift==0x00)//
//    {
//      MC321_WriteByte(0x801D,CheckSum);
//    }
//    else if(opt_shift==0x05)//
//    {
//      MC321_WriteByte(0x801E,CheckSum);
//    }
//  }
//  
////------------------------------Ƶ����֤ ȡд��оƬ���¶�У׼ֵ��Ƶ��У׼ֵ----------------------------  
//  if (DeviceConfig_xx.IRC_Select==0x01)//��ʽ����
//  //if (DeviceConfig_xx.IRC_Select==0x00)//���Դ���
//  {
//    ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+1+option_shift); //set ADDR
//    OPTION_FRT=ROMReadData/256;
//    OPTION_TEMPADJ=(OPTION_FRT&0xf8);
//    OPTION_FRT&=0x07;
//    OPTION_FRT2=ROMReadData%256;
//    ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+2+option_shift); //set ADDR
//    FdataL=ROMReadData%256;
//    FdataL&=0x01;
//    FdataL<<=5;
//    OPTION_TEMPADJ>>=3;
//    OPTION_TEMPADJ|=FdataL;
//    ReFlag=MC32P7022IRC_11Bit_ModeInCheck(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
//    ConfigTIM4();//�ر�Timer3��Timer4
//    if(ReFlag == 0) 
//    {
//      ERORR_VALUE=IRC_Value_false;
//      return 0;
//    }
//  }   
//  
////------------------------------�ٽ�ģʽ---------------------------- 
//  VDD30V_Off;
//  POWER_OFF(vpp00,vdd00);
//  Delay_1ms(1);    
//  MC321_MODEL_IN(vdd30);  
//  Delay_1ms(1);
//  CLN_VDD2GND_Off;
//  VPP12V_On;
//  Delay_1ms(10);  
//  Fdata=MC321_ReadByte(0XFFFF);
//  if (Fdata !=DeviceConfig_xx.MCU_ID) 
//  {
//    ERORR_VALUE=OTP_ModeIn_false;
//    return 0;
//  }
//  
////------------------------------rom ȫ���У��----------------------------   
//  if (DeviceConfig_xx.WriteEn & 0x0f)
//  {
//    if (WriteConfig_xx.AllCheckNeed > 0)
//    {
//      FM_CS_0;
//      FMReadMore(Addr_Flash_ROMStart+DeviceConfig_xx.RomFirAddr-1);
//      Fdata =Rxdata;
//      
//      ROMReadData=MC321_ReadByte(0x0000); //set ADDR
//      for(OTP_ADDR=DeviceConfig_xx.RomFirAddr;OTP_ADDR<=DeviceConfig_xx.RomEndAddr;)
//      {
//	SPI_ReadByte();
//	FdataL=Rxdata;
//	SPI_ReadByte();
//	FdataH=Rxdata;
//	Fdata=FdataH*0x100+FdataL;
//	
//	mcu_addr=OTP_ADDR/2;
//	OTP_ADDR=OTP_ADDR+2;
//	
//	if (WriteConfig_xx.AllCheckNeed == 2)
//	{
//          for(i=1;i<3;i++)
//          {
//            SuccessCnt = 0;
//            for(j=0;j<WriteConfig_xx.ReadCnt;j++)
//            {
//              ROMReadData=MC321_R_DiffMode(i);
//              if (ROMReadData==Fdata)
//              {
//                SuccessCnt++;
//              }
//              else
//              {
//              }
//              if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
//              {
//                break;
//              }
//            } 
//            
//            if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//�ڹ涨��ȡУ������´ﵽ��Ҫ��ĳɹ����� ��д�ɹ�
//            {
//            }
//            else
//            {
//              if(i==1)
//              {
//                ERORR_VALUE=MAGRIN1_ERROR;
//              }
//              else if(i==2)//������
//              {
//                ERORR_VALUE=OFF_MAGRIN_ERROR;
//              }
//              break;
//            }
//          }  
//          if (i==3)//3��ģʽ��ȡ����д�������һ�� ��д�ɹ� 2
//          {
//          }
//          else
//          {
//            OTP_ADDR= mcu_addr;
//            return 0;
//          }
//	}
//	else
//        {
//          SuccessCnt = 0;
//          for(j=0;j<WriteConfig_xx.ReadCnt;j++)
//          {
//            ROMReadData=MC321_R();
//            if (ROMReadData==Fdata)
//            {
//              SuccessCnt++;
//            }
//            else
//            {
//            }
//            if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
//            {
//              break;
//            }
//          } 
//          
//          if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//�ڹ涨��ȡУ������´ﵽ��Ҫ��ĳɹ����� ��д�ɹ�
//          {
//          }
//          else
//          {
//            ERORR_VALUE=NOMALL_ERROR;
//            OTP_ADDR= mcu_addr;
//            return 0;
//          }
//        }
//	
//	if (OTP_ADDR<DeviceConfig_xx.RomEndAddr)
//	{
//	  MC301_Instruct(AddrINC);
//	}
//      }
//      FM_CS_1;
//    }
//  }
//  
////------------------------------optionȫ���У��----------------------------    
//  if(DeviceConfig_xx.WriteEn & 0xf0)
//  {
//    if(WriteConfig_xx.AllCheckNeed > 0)
//    {
//      for(op_addr=0;op_addr<DeviceConfig_xx.OptionSize+1;op_addr++)
//      {
//	Fdata=WriteConfig_xx.OptionValue[op_addr];
//        Addres=WriteConfig_xx.OptionAddr[op_addr];
//
//	if (WriteConfig_xx.AllCheckNeed == 2)
//	{
//          for(i=1;i<3;i++)
//          {
//            SuccessCnt = 0;
//            for(j=0;j<WriteConfig_xx.ReadCnt;j++)
//            {
//              ROMReadData=MC321_ReadByte_DiffMode(i,Addres);
//              if (ROMReadData==Fdata)
//              {
//                SuccessCnt++;
//              }
//              else
//              {
//              }
//              if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
//              {
//                break;
//              }
//            } 
//            if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//�ڹ涨��ȡУ������´ﵽ��Ҫ��ĳɹ����� ��д�ɹ�
//            {
//            }
//            else
//            {
//              if(i==1)
//              {
//                ERORR_VALUE=OPTION_MAGRIN1_ERROR;
//              }
//              else if(i==2)//������
//              {
//                ERORR_VALUE=OPTION_OFF_MAGRIN_ERROR;
//              }
//              break;
//            }
//          }
//          if (i==3)//3��ģʽ��ȡ����д�������һ�� ��д�ɹ� 2
//          {
//          }
//          else
//          {
//            OTP_ADDR= Addres;
//            return 0;
//          } 
//	}
//	else
//        {
//          SuccessCnt = 0;
//	  for(j=0;j<WriteConfig_xx.ReadCnt;j++)
//	  {
//	    ROMReadData=MC321_ReadByte(Addres);
//	    if (ROMReadData==Fdata)
//	    {
//	      SuccessCnt++;
//	    }
//	    else
//	    {
//	    }
//	    if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
//	    {
//	      break;
//	    }
//	  } 
//	  if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//�ڹ涨��ȡУ������´ﵽ��Ҫ��ĳɹ����� ��д�ɹ�
//	  {
//	  }
//	  else
//	  {
//	    ERORR_VALUE=OPTION_NOMALL_ERROR;
//	    OTP_ADDR= Addres;
//            return 0;
//	  }
//        }
//      }
//    }
//  }
//  
////write protect bit
////------------------------------write protect bit----------------------------    
//  if(DeviceConfig_xx.WriteEn & 0xf0)
//  {  
//    FM_CS_0;
//    FMReadMore(Addr_Flash_Option+1);
//    Fdata =Rxdata;
//    SPI_ReadByte();//��flash��ȡһ���ֽ�
//    FdataL=Rxdata;
//    SPI_ReadByte();//�ٴ�flash��ȡһ���ֽ�
//    FdataH=Rxdata;
//    Fdata=FdataH*0x100+FdataL;//���һ���� 
//
//    ReCnt=WriteConfig_xx.ReWriteCnt;
//    while(ReCnt--) 
//    {	
//      ForceReCnt=WriteConfig_xx.ForceWriteCnt;      
//      if (Fdata != 0xffff)  // 
//      {
//        while(ForceReCnt--)
//        {
//          MC321_WriteByte(DeviceConfig_xx.OptionAddr+option_shift,Fdata);
//        } 
//      } 
//      if (WriteConfig_xx.AllCheckNeed == 2)
//      {
//        for(i=1;i<3;i++)
//        {
//          SuccessCnt = 0;
//          for(j=0;j<WriteConfig_xx.ReadCnt;j++)
//          {
//            ROMReadData=MC321_ReadByte_DiffMode(i,DeviceConfig_xx.OptionAddr+option_shift);
//            if (ROMReadData==Fdata)
//            {
//              SuccessCnt++;
//            }
//            else
//            {
//            }
//            if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
//            {
//              break;
//            }
//          } 
//          if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//�ڹ涨��ȡУ������´ﵽ��Ҫ��ĳɹ����� ��д�ɹ�
//          {
//          }
//          else
//          {
//            if(i==1)
//            {
//              ERORR_VALUE=OPTION_MAGRIN1_ERROR;
//            }
//            else if(i==2)//������
//            {
//              ERORR_VALUE=OPTION_OFF_MAGRIN_ERROR;
//            }
//            break;
//          }
//        }
//        if (i==3)//3��ģʽ��ȡ����д�������һ�� ��д�ɹ� 2
//        {
//          break;
//        }
//        else
//        {
//          OTP_ADDR= DeviceConfig_xx.OptionAddr+option_shift;
//          return 0;
//        } 
//      }  
//      else if(WriteConfig_xx.AllCheckNeed == 1)
//      {
//        SuccessCnt = 0;
//        for(j=0;j<WriteConfig_xx.ReadCnt;j++)
//        {
//          ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);
//          if (ROMReadData==Fdata)
//          {
//            SuccessCnt++;
//          }
//          else
//          {
//          }
//          if(SuccessCnt == WriteConfig_xx.ReadOkCnt)
//          {
//            break;
//          }
//        } 
//        if(SuccessCnt == WriteConfig_xx.ReadOkCnt)//�ڹ涨��ȡУ������´ﵽ��Ҫ��ĳɹ����� ��д�ɹ�
//        {
//        }
//        else
//        {
//          ERORR_VALUE=OPTION_NOMALL_ERROR;
//          OTP_ADDR= DeviceConfig_xx.OptionAddr+option_shift;
//          return 0;
//        }
//      }
//    }
//  }  
//  FM_CS_1;  
//  
//  VDD30V_Off;
//  POWER_OFF(vpp00,vdd00);
//  return 1;
//}
//

