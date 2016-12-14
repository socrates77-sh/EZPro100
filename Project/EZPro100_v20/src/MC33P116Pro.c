
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
#include "MC30P6060.h"
#include "MC32T8132.h"
#include "MC32E22.h" 
#include "MC32P7022.h"
#include "MC32P7212.h"
#include "MC32P7011.h"
#include "MC33P116Pro.h"
#include "MC32P7311.h"

u8 MC33P116IRC_11Bit_ModeInCheck(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i;
  u8 optL,optH;
  u16 mm;

  
  mm=0x400 ;//& (rdv<<8);
  MC301_Instruct(TestMod);

  for(i=0;i<5;i++)
  {
    MC321_IRC_INST(0X0000); //5 NOP
  }
  if (FMReadOne(addr_Flash_OptionProtec16K+11)==1)
  {
    optL =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    return 0;
  }

  if (FMReadOne(addr_Flash_OptionProtec16K+5)==1)
  {
      optH =Rxdata;
  }
  else
  {
      ERORR_VALUE=FM_Read_false;
      return 0;
  }
      
  mm=0x3c00+optH;

  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fb);     //opt 1 H
  mm=0x3c00+optL;  //IRC div temp
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fa);     //opt 1 L


  if (FMReadOne(addr_Flash_OptionProtec16K+9)==1)//3 H
  {
    optH =Rxdata;
    optH= optH | 0x18;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    return 0;
  }
  mm=0x3c00+optH;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57ff);     //3 H
  
   
  //OPTION_TEMPADJ=optH; //save temp adj value

  //mm=0x3c00+optH;
  
  optH=OPTION_TEMPADJ;
  optH<<=3;
  optH&=0xf8;
  mm=0x3c00+optH;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fd);     //2 H
    
    
  if (FMReadOne(addr_Flash_OptionProtec16K+8)==1)
  {
      optL =Rxdata;
  }
  else
  {
      ERORR_VALUE=FM_Read_false;
      return 0;
  }        
  optL&=0xfe;
  optL|=((OPTION_TEMPADJ&0x20)>>5);
  
  OPTION_TEMPADJ=optH;

  mm=0x3c00+optL;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fe);     //3 L
  
  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;

  MC321_IRC_INST(0x3c01);     //movai 0x01
  MC321_IRC_INST(0x57f7);     //movra 0x1f7 //set HIRC output to SDO
  
  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ; 

  mm=OPTION_FRT*256+OPTION_FRT2;
  MC31P11FTtest_11Bit(mm,OPTION_TEMPADJ);

  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    return 1;
  }  
  
  return 0;
}

u8 MC33P116IRC_11Bit_check(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u16 mm;
  u8 optL;
  
  if (FMReadOne(addr_Flash_OptionProtec16K+8)==1)
  {
      optL =Rxdata;
  }
  else
  {
      ERORR_VALUE=FM_Read_false;
      return 0;
  }        
  mm=0x3c00+optL;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fe);     //3 L  
  
  mm=OPTION_FRT*256+OPTION_FRT2;
  MC31P11FTtest_11Bit(mm,OPTION_TEMPADJ);

  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    return 1;
  }

  return 0;
}

u8 M33P116IRC_11Bit(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i;
  u8 optL,optH;
  u16 mm,temp0_FT,temp1_FT;
  u8 OPTION_TEMPADJ_2,OPTION_TEMPADJ_3;

  mm=0x400 ;//& (rdv<<8);
  MC301_Instruct(TestMod);

  for(i=0;i<5;i++)
  {
    MC321_IRC_INST(0X0000); //5 NOP
  }
  if (FMReadOne(addr_Flash_OptionProtec16K+11)==1)
  {
    optL =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    return 0;
  }

  if (FMReadOne(addr_Flash_OptionProtec16K+5)==1)
  {
      optH =Rxdata;
  }
  else
  {
      ERORR_VALUE=FM_Read_false;
      return 0;
  }
      
  mm=0x3c00+optH;

  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fb);     //opt 1 H
  mm=0x3c00+optL;  //IRC div temp
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fa);     //opt 1 L


  if (FMReadOne(addr_Flash_OptionProtec16K+9)==1)//3 H
  {
    optH =Rxdata;
    optH= optH | 0x18;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    return 0;
  }
  mm=0x3c00+optH;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57ff);     //3 H
  
  if((Rxdata&0x07)==0)//16M
  {
    OPTION_TEMPADJ=0x50;//0x0a
    OPTION_TEMPADJ_2=0x48;//0x09
    OPTION_TEMPADJ_3=0x58;//0x0b
  }
  else if((Rxdata&0x07)==1)//8M
  {
    OPTION_TEMPADJ=0x90;//0x0a
    OPTION_TEMPADJ_2=0x88;//0x09
    OPTION_TEMPADJ_3=0x98;//0x0b
  }
  else if((Rxdata&0x07)==2)//4M
  {
    OPTION_TEMPADJ=0x48;//0x0a
    OPTION_TEMPADJ_2=0x50;//0x09
    OPTION_TEMPADJ_3=0x40;//0x0b
  }
  else//2M、1M、455K
  {
    OPTION_TEMPADJ=0xf8;//0x0a
    OPTION_TEMPADJ_2=0xf0;//0x09
    OPTION_TEMPADJ_3=0xe8;//0x0b 
  }    
  //OPTION_TEMPADJ=optH; //save temp adj value

  //mm=0x3c00+optH;
  mm=0x3c00+OPTION_TEMPADJ;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fd);     //2 H
    
    
  if (FMReadOne(addr_Flash_OptionProtec16K+8)==1)
  {
      optL =Rxdata;
  }
  else
  {
      ERORR_VALUE=FM_Read_false;
      return 0;
  }        
  mm=0x3c00+optL;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fe);     //3 L
  
  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;

  MC321_IRC_INST(0x3c01);     //movai 0x01
  MC321_IRC_INST(0x57f7);     //movra 0x1f7 //set HIRC output to SDO
  
  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ; 

  mm=0x400;
  OPTION_FRT=0; //initial 
  IRC_MODEL=0;
  for(i=0;i<11;i++)
  {
    //mm=0xff;
    //M201FTSendData(IRC_Option,IRC_Addr,mm);
    MC31P11FTtest_11Bit(mm,OPTION_TEMPADJ);
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

  MC31P11FTtest_11Bit(mm+1,OPTION_TEMPADJ);
  temp0_FT=IRC_VALUE;

  MC31P11FTtest_11Bit(mm,OPTION_TEMPADJ);
  temp1_FT=IRC_VALUE;

  if((temp0_FT-IRC_FreqType)<(IRC_FreqType-temp1_FT))
  {
    mm=mm+1;
    IRC_VALUE=temp0_FT;
  }

  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    OPTION_FRT=mm/256;
    OPTION_FRT2=mm%256;
    return 1;
  }

  //第二个温度点
  OPTION_TEMPADJ=OPTION_TEMPADJ_2; 
  MC321_IRC_INST(0x0000) ;

  mm=0x400;
  OPTION_FRT=0; //initial 
  IRC_MODEL=1;
  for(i=0;i<11;i++)
  {
    //mm=0xff;
    //M201FTSendData(IRC_Option,IRC_Addr,mm);
    MC31P11FTtest_11Bit(mm,OPTION_TEMPADJ);
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

  MC31P11FTtest_11Bit(mm+1,OPTION_TEMPADJ);
  temp0_FT=IRC_VALUE;

  MC31P11FTtest_11Bit(mm,OPTION_TEMPADJ);
  temp1_FT=IRC_VALUE;

  if((temp0_FT-IRC_FreqType)<(IRC_FreqType-temp1_FT))
  {
    mm=mm+1;
    IRC_VALUE=temp0_FT;
  }

  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    OPTION_FRT=mm/256;
    OPTION_FRT2=mm%256;
    return 1;
  } 

  //第三个温度点
  OPTION_TEMPADJ=OPTION_TEMPADJ_3; 
  MC321_IRC_INST(0x0000) ;

  mm=0x400;
  OPTION_FRT=0; //initial 
  IRC_MODEL=2;
  for(i=0;i<11;i++)
  {
    //mm=0xff;
    //M201FTSendData(IRC_Option,IRC_Addr,mm);
    MC31P11FTtest_11Bit(mm,OPTION_TEMPADJ);
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

  MC31P11FTtest_11Bit(mm+1,OPTION_TEMPADJ);
  temp0_FT=IRC_VALUE;

  MC31P11FTtest_11Bit(mm,OPTION_TEMPADJ);
  temp1_FT=IRC_VALUE;

  if((temp0_FT-IRC_FreqType)<(IRC_FreqType-temp1_FT))
  {
    mm=mm+1;
    IRC_VALUE=temp0_FT;
  }

  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    OPTION_FRT=mm/256;
    OPTION_FRT2=mm%256;
    return 1;
  } 

  OPTION_FRT=0xff;
  OPTION_FRT2=0xff;
  return 0;
}

u8 M31P11IRC_11Bit(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i;
  u16 mm,temp0_FT,temp1_FT;

  mm=0x400 ;//& (rdv<<8);

  MC301_Instruct(TestMod);

  for(i=0;i<5;i++)
  {
    MC321_IRC_INST(0X0000); //5 NOP
  }
  OPTION_TEMPADJ=0x04;
//--------TEMPADJ=04H ------------------------------------------------------------
  MC321_IRC_INST(0x04c0) ;  //movai	0x01
  MC321_IRC_INST(0x0ac1) ;  //movra       fsr

  //mm=0x0408;    
  MC321_IRC_INST(0x0448) ;     //movai	0x38 ; //Fosc/8,FCPUS=10, LDO=1.7V ,TEMP=04
  MC321_IRC_INST(0x0af9) ; //movra	0xf9 ; 0xf9----0x8000[7:0] , 0xfa---0x8000[12:8]
  //opt01=opt01 & 0x10;
  //opt01=0x02;
  //mm=0x0400+opt01;
  MC321_IRC_INST(0x0402) ;     //movai	0x1c
  MC321_IRC_INST(0x0afa) ; //movra	0xfa

  MC321_IRC_INST(0x0401); 
  MC321_IRC_INST(0x0af8) ; 
  MC321_IRC_INST(0x0000) ; 
    
  mm=0x400;
  OPTION_FRT=0; //initial 
  IRC_MODEL=0;
  for(i=0;i<11;i++)
  {
          //mm=0xff;
          //M201FTSendData(IRC_Option,IRC_Addr,mm);
          MC31P11FTtest_11Bit(mm,OPTION_TEMPADJ);
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

  MC31P11FTtest_11Bit(mm+1,OPTION_TEMPADJ);
  temp0_FT=IRC_VALUE;

  MC31P11FTtest_11Bit(mm,OPTION_TEMPADJ);
  temp1_FT=IRC_VALUE;

  if((temp0_FT-IRC_FreqType)<(IRC_FreqType-temp1_FT))
  {
          mm=mm+1;
          IRC_VALUE=temp0_FT;
  }

  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
          OPTION_FRT=mm/256;
          OPTION_FRT2=mm%256;
          return 1;
  }


  OPTION_TEMPADJ=0x05;
//--------TEMPADJ=05H ------------------------------------------------------------
  MC321_IRC_INST(0x04c0) ;  //movai	0x01
  MC321_IRC_INST(0x0ac1) ;  //movra       fsr

  //opt00=opt00 & 0xf8;
  //opt00=opt00 | 0x88;
  //mm=0x0400+opt00;

  MC321_IRC_INST(0x04c8) ; //movai	0xc8 ; //Fosc/8,FCPUS=10
  MC321_IRC_INST(0x0af9) ; //movra	0xf9
  //opt01=opt01 & 0x10;
  //opt01=opt01 | 0x02;
  //mm=0x0400+opt01;
  MC321_IRC_INST(0x0402) ;     //movai	0x1c
  MC321_IRC_INST(0x0afa) ; //movra	0xfa


  MC321_IRC_INST(0x0401); 
  MC321_IRC_INST(0x0af8) ;

  MC321_IRC_INST(0x0000) ;

  mm=0x400;
  OPTION_FRT=0; //initial 
  IRC_MODEL=1;
  for(i=0;i<11;i++)
  {
          //mm=0xff;
          //M201FTSendData(IRC_Option,IRC_Addr,mm);
          MC31P11FTtest_11Bit(mm,OPTION_TEMPADJ);
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

  MC31P11FTtest_11Bit(mm+1,OPTION_TEMPADJ);
  temp0_FT=IRC_VALUE;

  MC31P11FTtest_11Bit(mm,OPTION_TEMPADJ);
  temp1_FT=IRC_VALUE;

  if((temp0_FT-IRC_FreqType)<(IRC_FreqType-temp1_FT))
  {
          mm=mm+1;
          IRC_VALUE=temp0_FT;
  }

  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
          OPTION_FRT=mm/256;
          OPTION_FRT2=mm%256;
          return 1;
  } 

  OPTION_TEMPADJ=0x03;
//--------TEMPADJ=03H ------------------------------------------------------------
  MC321_IRC_INST(0x04c0) ;  //movai	0x01
  MC321_IRC_INST(0x0ac1) ;  //movra       fsr

  MC321_IRC_INST(0x04c8) ; //movai	0xc8 ; //Fosc/8,FCPUS=10
  MC321_IRC_INST(0x0af9) ; //movra	0xf9
  
  MC321_IRC_INST(0x0401) ;     //movai	0x1c
  MC321_IRC_INST(0x0afa) ; //movra	0xfa


  MC321_IRC_INST(0x0401); 
  MC321_IRC_INST(0x0af8) ;

  MC321_IRC_INST(0x0000) ;

  mm=0x400;
  OPTION_FRT=0; //initial 
  IRC_MODEL=2;
  for(i=0;i<11;i++)
  {
    //mm=0xff;
    //M201FTSendData(IRC_Option,IRC_Addr,mm);
    MC31P11FTtest_11Bit(mm,OPTION_TEMPADJ);
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

  MC31P11FTtest_11Bit(mm+1,OPTION_TEMPADJ);
  temp0_FT=IRC_VALUE;

  MC31P11FTtest_11Bit(mm,OPTION_TEMPADJ);
  temp1_FT=IRC_VALUE;

  if((temp0_FT-IRC_FreqType)<(IRC_FreqType-temp1_FT))
  {
          mm=mm+1;
          IRC_VALUE=temp0_FT;
  }

  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
          OPTION_FRT=mm/256;
          OPTION_FRT2=mm%256;
          return 1;
  } 

  OPTION_FRT=0xff;
  OPTION_FRT2=0xff;
  return 0;
}

u8 M31P11IRC_11Bit_Check(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u16 mm;

  if(OPTION_TEMPADJ==0x04)
  {
  //--------TEMPADJ=04H ------------------------------------------------------------
    MC321_IRC_INST(0x04c0) ;  //movai	0x01
    MC321_IRC_INST(0x0ac1) ;  //movra       fsr
   
    MC321_IRC_INST(0x0448) ;     //movai	0x38 ; //Fosc/8,FCPUS=10, LDO=1.7V ,TEMP=04
    MC321_IRC_INST(0x0af9) ; //movra	0xf9 ; 0xf9----0x8000[7:0] , 0xfa---0x8000[12:8]

    MC321_IRC_INST(0x0402) ;     //movai	0x1c
    MC321_IRC_INST(0x0afa) ; //movra	0xfa
  }
  else if(OPTION_TEMPADJ==0x05)
  {
  //--------TEMPADJ=05H ------------------------------------------------------------
    MC321_IRC_INST(0x04c0) ;  //movai	0x01
    MC321_IRC_INST(0x0ac1) ;  //movra       fsr
    
    MC321_IRC_INST(0x04c8) ; //movai	0xc8 ; //Fosc/8,FCPUS=10
    MC321_IRC_INST(0x0af9) ; //movra	0xf9
    
    MC321_IRC_INST(0x0402) ;     //movai	0x1c
    MC321_IRC_INST(0x0afa) ; //movra	0xfa
  }
  else if(OPTION_TEMPADJ==0x03)
  {
  //--------TEMPADJ=03H ------------------------------------------------------------
    MC321_IRC_INST(0x04c0) ;  //movai	0x01
    MC321_IRC_INST(0x0ac1) ;  //movra       fsr

    MC321_IRC_INST(0x04c8) ; //movai	0xc8 ; //Fosc/8,FCPUS=10
    MC321_IRC_INST(0x0af9) ; //movra	0xf9
  
    MC321_IRC_INST(0x0401) ;     //movai	0x1c
    MC321_IRC_INST(0x0afa) ; //movra	0xfa
  }
  MC321_IRC_INST(0x0401); 
  MC321_IRC_INST(0x0af8) ; 
  MC321_IRC_INST(0x0000) ; 
    
  mm=OPTION_FRT*256+OPTION_FRT2;
  MC31P11FTtest_11Bit(mm,OPTION_TEMPADJ);       
  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    //OPTION_FRT=mm/256;
    //OPTION_FRT2=mm%256;
    return 1;
  }

  OPTION_FRT=0xff;
  OPTION_FRT2=0xff;
  return 0;
}

u8 M31P11IRC_11Bit_ModeInCheck(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i;
  u16 mm;
  
  mm=0x400 ;//& (rdv<<8);

  MC301_Instruct(TestMod);

  for(i=0;i<5;i++)
  {
    MC321_IRC_INST(0X0000); //5 NOP
  }
  
  if(OPTION_TEMPADJ==0x04)
  {
  //--------TEMPADJ=04H ------------------------------------------------------------
    MC321_IRC_INST(0x04c0) ;  //movai	0x01
    MC321_IRC_INST(0x0ac1) ;  //movra       fsr
   
    MC321_IRC_INST(0x0448) ;     //movai	0x38 ; //Fosc/8,FCPUS=10, LDO=1.7V ,TEMP=04
    MC321_IRC_INST(0x0af9) ; //movra	0xf9 ; 0xf9----0x8000[7:0] , 0xfa---0x8000[12:8]

    MC321_IRC_INST(0x0402) ;     //movai	0x1c
    MC321_IRC_INST(0x0afa) ; //movra	0xfa
  }
  else if(OPTION_TEMPADJ==0x05)
  {
  //--------TEMPADJ=05H ------------------------------------------------------------
    MC321_IRC_INST(0x04c0) ;  //movai	0x01
    MC321_IRC_INST(0x0ac1) ;  //movra       fsr
    
    MC321_IRC_INST(0x04c8) ; //movai	0xc8 ; //Fosc/8,FCPUS=10
    MC321_IRC_INST(0x0af9) ; //movra	0xf9
    
    MC321_IRC_INST(0x0402) ;     //movai	0x1c
    MC321_IRC_INST(0x0afa) ; //movra	0xfa
  }
  else if(OPTION_TEMPADJ==0x03)
  {
  //--------TEMPADJ=03H ------------------------------------------------------------
    MC321_IRC_INST(0x04c0) ;  //movai	0x01
    MC321_IRC_INST(0x0ac1) ;  //movra       fsr

    MC321_IRC_INST(0x04c8) ; //movai	0xc8 ; //Fosc/8,FCPUS=10
    MC321_IRC_INST(0x0af9) ; //movra	0xf9
  
    MC321_IRC_INST(0x0401) ;     //movai	0x1c
    MC321_IRC_INST(0x0afa) ; //movra	0xfa
  }
  MC321_IRC_INST(0x0401); 
  MC321_IRC_INST(0x0af8) ; 
  MC321_IRC_INST(0x0000) ; 
    
  mm=OPTION_FRT*256+OPTION_FRT2;
  MC31P11FTtest_11Bit(mm,OPTION_TEMPADJ);       
  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  {
    //OPTION_FRT=mm/256;
    //OPTION_FRT2=mm%256;
    return 1;
  }

  OPTION_FRT=0xff;
  OPTION_FRT2=0xff;
  return 0;
}

u8 MC32P5312IRC_11Bit_ModeInCheck(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u8 i;
  u8 optL,optH,opt_2;
  u16 mm;
  u8 OPTION_FAS;
  u8 OPTION_TEMPADJ_2;
  
  FM_CS_0;
  if (FMReadOne(Addr_Flash_OptionProtec+4)==1)//option 2 低字节
  {
    optL =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }
  if (FMReadOne(Addr_Flash_OptionProtec+5)==1)//option 2 高字节
  {
    optH =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }    
  
  opt_2 = optL;
  OPTION_FAS = optH & 0x07; //确定是16M 8M 4M 2M 1M 455K哪一种，再确定两次校验要用的温度校准值OPTION_TEMPADJ_1、OPTION_TEMPADJ_2
   
  MC301_Instruct(TestMod);

  for(i=0;i<5;i++)
  {
    MC321_IRC_INST(0X0000); //5 NOP
  }
  if(OPTION_FAS == 0)//16M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;
    optL = 0x70;
  }
  else if(OPTION_FAS == 1)//8M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;   
    optL = 0x60;
  }
  else if(OPTION_FAS == 2)//4M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
    optL = 0x50;
  }
  else if(OPTION_FAS == 3)//2M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
    optL = 0x40;
  }   
  else if(OPTION_FAS == 4)//1M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
    optL = 0x30;
  }   
  else if(OPTION_FAS == 5)//455K
  {
    IRC_FreqType = 2275;
    IRC_FreqMin = 2268;
    IRC_FreqMax = 2282;    
    optL = 0x10;
  }   
  
  mm=0x3c00+optL;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57f9);     //opt0 L
  
  optH = (optH & 0x3f) | 0xd8;//opt2 FAS FDS RCSMTB相关位 注意：校验时 要求FDS要写入固定值11
  mm=0x3c00+optH;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fe);     //opt2 H
  
  optL = (opt_2 & 0x02) | 0xfd;//VSDEL相关位
  mm=0x3c00+optL;
  MC321_IRC_INST(mm);
  MC321_IRC_INST(0x57fd);     //opt2 L
  
  OPTION_TEMP = optH;
  OPTION_TEMP = (OPTION_TEMP<<8)&0xff00;
  OPTION_TEMP = OPTION_TEMP+optL;
//  Fdata = optH<<8 + optL;
  
  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;

  MC321_IRC_INST(0x3c01);     //movai 0x01
  MC321_IRC_INST(0x57f8);     //movra 0x1f8 //set HIRC output to SDO

  MC321_IRC_INST(0x0000) ; 
  MC321_IRC_INST(0x0000) ;   

  mm=OPTION_FRT*256+OPTION_FRT2;
  OPTION_TEMPADJ_2=OPTION_TEMPADJ;
    
  MC32P5312FTtest_11Bit(mm,OPTION_TEMPADJ_2);
  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  { 
    return 1;//
  } 
  OPTION_FRT=0xff;
  OPTION_FRT2=0xff;
  OPTION_TEMPADJ = 0xff;  
  return 0; 
}


u8 MC32P5312IRC_11Bit_check(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
{
  u16 mm;
  u8 OPTION_TEMPADJ_2;
  u8 optH;
  
  if (FMReadOne(Addr_Flash_OptionProtec+5)==1)//option 2 高字节
  {
    optH =Rxdata;
  }
  else
  {
    ERORR_VALUE=FM_Read_false;
    FM_CS_1;
    return 0;
  }    
  optH &= 0x07; //确定是16M 8M 4M 2M 1M 455K哪一种，再确定两次校验要用的温度校准值OPTION_TEMPADJ_1、OPTION_TEMPADJ_2
  if(optH == 0)//16M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;
  }
  else if(optH == 1)//8M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;   
  }
  else if(optH == 2)//4M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
  }
  else if(optH == 3)//2M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
  }   
  else if(optH == 4)//1M
  {
    IRC_FreqType = 1250;
    IRC_FreqMin = 1246;
    IRC_FreqMax = 1254;    
  }   
  else if(optH == 5)//455K
  {
    IRC_FreqType = 2275;
    IRC_FreqMin = 2268;
    IRC_FreqMax = 2282;    
  }   

  
  mm=OPTION_FRT*256+OPTION_FRT2;
  OPTION_TEMPADJ_2=OPTION_TEMPADJ;
    
  MC32P5312FTtest_11Bit(mm,OPTION_TEMPADJ_2);
  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
  { 
    return 1;//
  } 
  OPTION_FRT=0xff;
  OPTION_FRT2=0xff;
  OPTION_TEMPADJ = 0xff;  
  return 0;  
}



u8 MC33P116_Program()
{
  u8    i,j,ReCnt=0;
  u16   Fdata,ROMReadData;
  u8	FdataL,FdataH;
  u8    ReFlag=0;

  u8    id_length=0;
  u16   id_addr0,id_addr1,id_addr2,id_addr3,mcu_addr;
  u32   id_end,current_id,max_id;
  
  u8    ForceReCnt=0;  
  u8    SuccessCnt;  
  u8    op_addr=0;
  u8    option_shift=0;  
  u16   CheckSum=0,CheckSum_c=0;  
  u16   Addres;
  u8    IRC_Empty=0;//
  
  u16   CheckAddr;  
  
  if(DeviceConfig_xx.MCU_ID==0x3316)
  {
    WriteConfig_xx.RomBit=0xffff;
    WriteConfig_xx.ProTime=1;
    WriteConfig_xx.ReadProAddrAndData=1;
    WriteConfig_xx.Page=1;
    WriteConfig_xx.Addr_Flash_OptionStart=addr_Flash_OptionProtec16K+2;
    WriteConfig_xx.SecurityOption=0;
    WriteConfig_xx.SecurityBit=15;
    WriteConfig_xx.WriteCheckNeed=0;
    WriteConfig_xx.ReadCnt=1;
    WriteConfig_xx.ReadOkCnt=1;
    WriteConfig_xx.ReWriteCnt=3;
    WriteConfig_xx.ForceWriteCnt=1;
    WriteConfig_xx.AllCheckNeed=2;     
  }                
  else if(DeviceConfig_xx.MCU_ID==0x3111)
  {
    WriteConfig_xx.RomBit=0x1fff;
    WriteConfig_xx.ProTime=1;
    WriteConfig_xx.ReadProAddrAndData=0;
    WriteConfig_xx.Page=0;
    WriteConfig_xx.Addr_Flash_OptionStart=Addr_Flash_OptionProtec;
    WriteConfig_xx.SecurityOption=0;
    WriteConfig_xx.SecurityBit=12;    
    WriteConfig_xx.WriteCheckNeed=0;
    WriteConfig_xx.ReadCnt=1;
    WriteConfig_xx.ReadOkCnt=1;
    WriteConfig_xx.ReWriteCnt=3;
    WriteConfig_xx.ForceWriteCnt=1;
    WriteConfig_xx.AllCheckNeed=2;     
  }
  else if(DeviceConfig_xx.MCU_ID==0x5312)
  {
    WriteConfig_xx.RomBit=0xffff;
    WriteConfig_xx.ProTime=1;
    WriteConfig_xx.ReadProAddrAndData=1;
    WriteConfig_xx.Page=0;
    WriteConfig_xx.Addr_Flash_OptionStart=Addr_Flash_OptionProtec;
    WriteConfig_xx.SecurityOption=0;
    WriteConfig_xx.SecurityBit=15;    
    WriteConfig_xx.WriteCheckNeed=0;
    WriteConfig_xx.ReadCnt=1;
    WriteConfig_xx.ReadOkCnt=1;
    WriteConfig_xx.ReWriteCnt=3;
    WriteConfig_xx.ForceWriteCnt=1;
    WriteConfig_xx.AllCheckNeed=2;       
  }
  else if(DeviceConfig_xx.MCU_ID==0x7311)
  {
    WriteConfig_xx.RomBit=0xffff;
    WriteConfig_xx.ProTime=1;
    WriteConfig_xx.ReadProAddrAndData=0;
    WriteConfig_xx.Page=0;
    WriteConfig_xx.Addr_Flash_OptionStart=Addr_Flash_OptionProtec;
    WriteConfig_xx.SecurityOption=0;
    WriteConfig_xx.SecurityBit=15;     
    WriteConfig_xx.WriteCheckNeed=0;
    WriteConfig_xx.ReadCnt=1;
    WriteConfig_xx.ReadOkCnt=1;
    WriteConfig_xx.ReWriteCnt=3;
    WriteConfig_xx.ForceWriteCnt=1;
    WriteConfig_xx.AllCheckNeed=1;
  }
  else if(DeviceConfig_xx.MCU_ID==0x7022)
  {
    WriteConfig_xx.RomBit=0xffff;
    WriteConfig_xx.ProTime=1;
    WriteConfig_xx.ReadProAddrAndData=0;
    WriteConfig_xx.Page=1;
    WriteConfig_xx.Addr_Flash_OptionStart=Addr_Flash_OptionProtec;
    WriteConfig_xx.SecurityOption=0;
    WriteConfig_xx.SecurityBit=15;     
    WriteConfig_xx.WriteCheckNeed=0;
    WriteConfig_xx.ReadCnt=1;
    WriteConfig_xx.ReadOkCnt=1;
    WriteConfig_xx.ReWriteCnt=3;
    WriteConfig_xx.ForceWriteCnt=1;
    WriteConfig_xx.AllCheckNeed=2;  
  }
//先进模式  
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  ConfigTIM4();
  Delay_1ms(5);
  //power on & model in
  MC321_MODEL_IN(vdd30);
  Delay_1ms(1);
  CLN_VDD2GND_Off;
  VPP12V_On;
  Delay_1ms(10);
  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID)
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  }
  
  
//IRC adjust
  if (DeviceConfig_xx.IRC_Select==0x01)//正式代码
  //if (DeviceConfig_xx.IRC_Select==0x00)//调试代码
  {
    if(DeviceConfig_xx.MCU_ID==0x3316)
    {
      ReFlag=M33P116IRC_11Bit(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
      if(ReFlag==1)
      {
        ReFlag=MC33P116IRC_11Bit_check(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
      }
    }
    else if(DeviceConfig_xx.MCU_ID==0x5312)
    {
      ReFlag=MC32P5312IRC_11Bit(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
      if(ReFlag==1)
      {
        ReFlag=MC32P5312IRC_11Bit_check(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
      }
    }
    else if(DeviceConfig_xx.MCU_ID==0x3111)
    {
      ReFlag=M31P11IRC_11Bit(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
      if(ReFlag==1)
      {
        ReFlag=M31P11IRC_11Bit_Check(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
      }      
    }
    else if(DeviceConfig_xx.MCU_ID==0x7311)
    {
      ReFlag=MC32P7311IRC_10Bit(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
      if(ReFlag==1)
      {
        ReFlag=MC32P7311IRC_10Bit_Check(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
      }      
    }
    else if(DeviceConfig_xx.MCU_ID==0x7022)
    {
      ReFlag=MC32P7022IRC_11Bit(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
      if(ReFlag==1)
      {
        ReFlag=MC32P7022IRC_11Bit_Check(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
      }
    }
    ConfigTIM4();
    if(ReFlag == 0) 
    {
      ERORR_VALUE=IRC_Value_false;
      return 0;
    }
  }      
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  ConfigTIM4();
  Delay_1ms(5);
  //power on & model in
  MC321_MODEL_IN(vdd30);
  Delay_1ms(1);
  CLN_VDD2GND_Off;
  VPP12V_On;
  Delay_1ms(10);
  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID)
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  }

//Write Page
  if(WriteConfig_xx.Page==0x01)
  {
    if(DeviceConfig_xx.MCU_ID==0x3316)
    {
      id_end=WriteConfig_xx.Addr_Flash_OptionStart-2;
      if (FMReadOne(id_end)==1)
      {
        FdataL =Rxdata;
      }
      else
      {
        return 0;
      }
      Fdata=FdataL;
      if((Fdata&0x70)==0x30)//16k
      {
        CheckAddr = 0x800D;
      }
      else if((Fdata&0x70)==0x50)//8k
      {
        if((Fdata&0x0C)==0x04)
        {
          CheckAddr = 0x800C;//第一页
        }
        else
        {
          CheckAddr = 0x800D;//第二页
        }
      }
      else if((Fdata&0x70)==0x60)//4k模式             
      {
        CheckAddr = 0;     //4K烧录不支持checksum   
      }
    }
    else if(DeviceConfig_xx.MCU_ID==0x7022)
    {
      //FM_CS_0;
      if (FMReadOne(Addr_Flash_OptionProtec)==1)//
      {
        FdataL =Rxdata;
      }
      else
      {
        ERORR_VALUE=FM_Read_false;
        FM_CS_1;
        return 0;
      }
      if (FMReadOne(Addr_Flash_OptionProtec+1)==1)//
      {
        FdataH =Rxdata;
      }
      else
      {
        ERORR_VALUE=FM_Read_false;
        FM_CS_1;
        return 0;
      }    
      Fdata=FdataH*0x100+FdataL;//组成一个字  
      if((Fdata&0x0070) == 0x0030)//1次烧写模式
      {
        DeviceConfig_xx.RomEndAddr = 0x1fbf;
        option_shift = 0;
      }
      else if((Fdata&0x0070) == 0x0050)//2次烧写模式
      {
        DeviceConfig_xx.RomEndAddr = 0xfbf;
        if((Fdata&0x000c)==0x0004)//第1页
        {
          option_shift = 0;
        }
        if((Fdata&0x000c)==0x0000)//第2页
        {
          option_shift = 5;
        }    
      }
      else if((Fdata&0x0070) == 0x0060)//4次烧写模式
      {
        DeviceConfig_xx.RomEndAddr = 0x7bf;
        if((Fdata&0x000f)==0x0007)//第1页
        {
          option_shift = 0;
        }
        else if((Fdata&0x000f)==0x0003)//第2页
        {
          option_shift = 5;
        }    
        else if((Fdata&0x000f)==0x0001)//第3页
        {
          option_shift = 10;
        }  
        else if((Fdata&0x000f)==0x0000)//第4页
        {
          option_shift = 15;
        }  
      }
      //FM_CS_1;       
    }
    WriteConfig_xx.OptionValue[0]=Fdata;
    WriteConfig_xx.OptionAddr[0]=0x8000;
    ReCnt=WriteConfig_xx.ReWriteCnt;
    while(ReCnt--) 
    {	
      ForceReCnt=WriteConfig_xx.ForceWriteCnt;
      if (Fdata != 0xffff)  // 
      {
        while(ForceReCnt--)
        {
          MC321_WriteByte(0x8000,Fdata);
        }
      }    
      if (WriteConfig_xx.AllCheckNeed == 2)
      {
        for(i=1;i<3;i++)
        {
          SuccessCnt = 0;
          for(j=0;j<WriteConfig_xx.ReadCnt;j++)
          {
            ROMReadData=MC321_ReadByte_DiffMode(i,0x8000);
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
          OTP_ADDR= 0x8000;
          return 0;
        } 
      }  
      else if(WriteConfig_xx.AllCheckNeed == 1)
      {
        SuccessCnt = 0;
        for(j=0;j<WriteConfig_xx.ReadCnt;j++)
        {
          ROMReadData=MC321_ReadByte(0x8000);
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
          ERORR_VALUE=OPTION_NOMALL_ERROR;
          OTP_ADDR= 0x8000;
          return 0;
        }
      }
    }
  
  //写完page再进模式
    VDD30V_Off;
    POWER_OFF(vpp00,vdd00);
    Delay_1ms(10);

    MC321_MODEL_IN(vdd30);
    Delay_1ms(1);

    CLN_VDD2GND_Off;
    VPP12V_On;
    //VPP_IO_On;
    Delay_1ms(10);
    Fdata=MC321_ReadByte(0XFFFF);
    if (Fdata !=DeviceConfig_xx.MCU_ID)
    {
      ERORR_VALUE=OTP_ModeIn_false;
      return 0;
    }  
  } 
//Write ROM
  if (DeviceConfig_xx.WriteEn & 0x0f)
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
      SPI_ReadByte();
      FdataL=Rxdata;
      CheckSum +=FdataL;
      CheckSum_c += FdataL;
      SPI_ReadByte();
      FdataH=Rxdata;
      CheckSum +=FdataH;
      CheckSum_c += (FdataH<<8);
      Fdata=FdataH*0x100+FdataL;
      Fdata&=WriteConfig_xx.RomBit;
      //Fdata=Fdata & 0x3fff;
      //2012.09.12 dynamic id
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
      //------------------------------------------------
      Rxdata=Fdata;
      Fdata&=WriteConfig_xx.RomBit;
      ReCnt=WriteConfig_xx.ReWriteCnt;
      while(ReCnt--)  //start wr rom
      {	 
        ForceReCnt=WriteConfig_xx.ForceWriteCnt;
        if (Fdata!=WriteConfig_xx.RomBit)
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
        if (WriteConfig_xx.ReadProAddrAndData==1)
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
//Write OPTION  
  if(DeviceConfig_xx.WriteEn & 0xf0)
  {
    FM_CS_0;
    //FMReadMore(WriteConfig_xx.Addr_Flash_OptionStart+3);
    FMReadMore(WriteConfig_xx.Addr_Flash_OptionStart+WriteConfig_xx.Page*2-1);    
    Fdata =Rxdata;
    
    ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr); //set ADDR
    for(op_addr=0;op_addr<DeviceConfig_xx.OptionSize;op_addr++)//
    {
      SPI_ReadByte();//从flash中取一个字节
      FdataL=Rxdata;
      SPI_ReadByte();//再从flash中取一个字节
      FdataH=Rxdata;
      Fdata=FdataH*0x100+FdataL;//组成一个字
      
      Fdata&=WriteConfig_xx.RomBit;
      
      if(DeviceConfig_xx.MCU_ID==0x3111)
      {
        //option0 
        if(op_addr==0)
        {          
          Fdata|=(0x0001<<WriteConfig_xx.SecurityBit);
          ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);
          Fdata&=0xf07f;
          ROMReadData&=0xf80;
          Fdata|=ROMReadData;
          if(ROMReadData==0xf80)//
          {//温度校准值为空
            ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift+1);
            if((ROMReadData&0x7ff)==0x7ff)//
            {//频率校准值为空
              IRC_Empty=1;
              Fdata|=(0x0001<<WriteConfig_xx.SecurityBit);
              Fdata&=0x107f;
              FdataH=OPTION_TEMPADJ;
              FdataH>>=1;
              FdataH&=0x0f;
              FdataL=OPTION_TEMPADJ;
              FdataL<<=7;
              FdataL&=0x80;
              Fdata=Fdata|(FdataH*256)|FdataL;
            }
          }          
        } 
        else if(op_addr==1)
        {
          ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);
          ROMReadData&=0x7ff;
          Fdata&=0xf800;
          Fdata|=ROMReadData;
          if(IRC_Empty)
          {
            FdataH&=0x18;
            FdataH|=OPTION_FRT;
            FdataL=OPTION_FRT2;
            Fdata=FdataH*256+FdataL;
          }
        }
      }
      else if(DeviceConfig_xx.MCU_ID==0x3316)
      {
        //option0 
        if(op_addr==0)
        {          
          //Fdata|=0x8000;//不写加密位
          Fdata|=(0x0001<<WriteConfig_xx.SecurityBit);
        }      
        //option1 //频率校准值 和 温度校准值  
        if(op_addr==1)
        {          
          ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);
          Fdata = ROMReadData;
          if(ROMReadData==0xffff)//
          {
            ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift+1);
            if((ROMReadData&0x0001)==0x0001)//
            {
              IRC_Empty=1;
              FdataH =OPTION_FRT; //IRC from QC test
              FdataL =OPTION_FRT2;            
              FdataH=OPTION_TEMPADJ & 0xf8;
              FdataH =FdataH | OPTION_FRT;
              Fdata=FdataH*0x100+FdataL;
            }
          }
        }
        else if(op_addr==2)//温度校准值  
        {
          ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);
          Fdata=(Fdata&0xfffe)|(ROMReadData&0x0001);
          if(IRC_Empty)//
          {
            Fdata=FdataH*0x100+FdataL;//温度校准值最高位用上位机传下来的值
          }
        }
      }
      else if(DeviceConfig_xx.MCU_ID==0x5312)
      {
        //option0 
        if(op_addr==0)
        {          
          //Fdata|=0x8000;//不写加密位
          Fdata|=(0x0001<<WriteConfig_xx.SecurityBit);
        }      
        //option1 //频率校准值 和 温度校准值  
        if(op_addr==1)
        {          
          ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);
          Fdata = ROMReadData;
          if(ROMReadData==0xffff)//
          {
            ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift+1);
            if((ROMReadData&0x0001)==0x0001)//
            {
              IRC_Empty=1;
              FdataH =OPTION_FRT; //IRC from QC test
              FdataL =OPTION_FRT2;
              FdataH = (OPTION_TEMPADJ<<3)&0xf8 | OPTION_FRT ;      
              Fdata=FdataH*0x100+FdataL;
            }
          }
        }
        else if(op_addr==2)//温度校准值  
        {
          ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);
          Fdata=(Fdata&0xfffe)|(ROMReadData&0x0001);
          if(IRC_Empty)//
          {
            FdataL = (FdataL&0xfe) | ((OPTION_TEMPADJ&0x20)>>5);//OPTION2 温度校准位bit6
            Fdata=FdataH*0x100+FdataL;
          }
        }    
      }
      else if(DeviceConfig_xx.MCU_ID==0x7311)
      {
        //option0 
        if(op_addr==0)
        {          
          Fdata|=0x8000;//不写加密位
        }      
        //option1 //写频率校准值 
        if(op_addr==1)
        {          
          ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr);
          if((ROMReadData&0x7ff)==0x07ff)//没写过频率校准值
          {
            Fdata = (Fdata&0xf800)|(OPTION_FRT*256)|OPTION_FRT2;
          }
          else
          {
            Fdata = (Fdata&0xf800)|(ROMReadData&0x7ff);
          }
          //Fdata &= 0xDFFF;
        }
        else if(op_addr==2)//写温度校准值 及Vref
        {//
          ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr);
          if((ROMReadData&0x00ff) == 0x00ff)//没写过adc reference校准值 
          {
            ROMReadData = (ROMReadData&0xff00)|(0x00ff&MC321_ReadByte(0x8005));//取成测adc reference校准值
            if((ROMReadData&0x00ff) == 0x00ff)//没有成测adc reference校准值
            {
              ROMReadData = (ROMReadData&0xff00)|(0x00ff&MC321_ReadByte(0x8004));//取中测adc reference校准值
              if((ROMReadData&0x00ff) == 0x00ff)//也没有中测adc reference校准值
              {
                ERORR_VALUE=OPTION_Write_false;//没写过adc reference频率校准值 也没有成测adc reference值 也没有中测adc reference值 报错
                return 0;
              }
            }          
          }      
          if((ROMReadData&0x3f00) == 0x3f00)//没写过温度校准值 
          {
            ROMReadData = (ROMReadData&0xc0ff)|(TADJ<<8);
          }        
          Fdata = ROMReadData;//
        }
        else if(op_addr==3)//VBG
        {//
          ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr);
          if((ROMReadData&0x00ff) == 0x00ff)//没写过VBG校准值 
          {
            ROMReadData = (ROMReadData&0xff00)|((0xff00&MC321_ReadByte(0x8005))>>8);//取成测VBG校准值
            if((ROMReadData&0x00ff) == 0x00ff)//没有成测VBG校准值
            {
              ROMReadData = (ROMReadData&0xff00)|((0xff00&MC321_ReadByte(0x8004))>>8);//取中测VBG校准值
              if((ROMReadData&0x00ff) == 0x00ff)//也没有中测VBG校准值
              {
                ERORR_VALUE=OPTION_Write_false;//没写过VBG频率校准值 也没有成测VBG值 也没有中测VBG值 报错
                return 0;
              }
            }          
          }        
          Fdata = ROMReadData;//
        }
                
      }
      else if(DeviceConfig_xx.MCU_ID==0x7022)
      {
        //option0 
        if(op_addr==0)
        {          
          Fdata|=0x8000;//不写加密位
        }      
        //option1 
        if(op_addr==1)        
        {
          Fdata = OPTION_FRT*256+OPTION_FRT2;
          /*测试版
          Fdata = (Fdata&0xf800) + temp;       //校准得到的频率校准值
          ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+opt_shift);
          if(ROMReadData!=0xffff)
          {
            Fdata=ROMReadData;
          }    
          */
          //正式版
          Fdata = (OPTION_TEMPADJ<<3)*256+Fdata;
          ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);
          if(ROMReadData!=0xffff)//校验并写过
          {
            Fdata=ROMReadData;//读出来再写进去
            IRC_Empty = 0;//芯片频率校准值被写过
          }    
          else 
          {
            IRC_Empty = 1;
          }
        }
        //option2                       
        if(op_addr==2)//温度校准位最高位 测试阶段 根据上位机写  正式版本写校准得到的值
        {                  
          ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift-1);
          if(IRC_Empty==0)//芯片频率校准值被写过
          {
            ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);
            Fdata=(Fdata&0xfffe)|(ROMReadData&0x0001);
          }    
          else if(IRC_Empty==1)
          {
            FdataL = OPTION_TEMPADJ;
            FdataL = FdataL>>5;
            Fdata = (Fdata&0xfffe)+FdataL;       
          }      
        }

        if(op_addr==3)//
        {          
          ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);
          if(ROMReadData==0xffff)//没写过Vref
          {
            ROMReadData = MC321_ReadByte(0x8018);
            if(ROMReadData==0xffff)//没有成测值
            {
              ROMReadData = MC321_ReadByte(0x8015);
              if(ROMReadData==0xffff)//没有中测值
              {
                ERORR_VALUE=OPTION_Write_false;//没写过adc reference频率校准值 也没有成测adc reference值 也没有中测adc reference值 报错
                return 0;
              }
            }
            Fdata = ROMReadData&0x00ff;
            
            ROMReadData = MC321_ReadByte(0x8019);
            if(ROMReadData==0xffff)//没有成测值
            {
              ROMReadData = MC321_ReadByte(0x8016);
              if(ROMReadData==0xffff)//没有中测值
              {
                ERORR_VALUE=OPTION_Write_false;//没写过adc reference频率校准值 也没有成测adc reference值 也没有中测adc reference值 报错
                return 0;
              }
            } 
            ROMReadData = ROMReadData<<8;
            Fdata = Fdata+ROMReadData;            
          }
          else
          {
            Fdata=ROMReadData;
          }
        }

        if(op_addr==4)//
        {          
          ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);
          if(ROMReadData==0xffff)//没写过Vref
          {
            ROMReadData = MC321_ReadByte(0x801A);
            if(ROMReadData==0xffff)//没有成测值
            {
              ROMReadData = MC321_ReadByte(0x8017);
              if(ROMReadData==0xffff)//没有中测值
              {
                ERORR_VALUE=OPTION_Write_false;//没写过adc reference频率校准值 也没有成测adc reference值 也没有中测adc reference值 报错
                return 0;
              }
            }
            ROMReadData = ROMReadData&0x00ff;
            ROMReadData = ROMReadData<<8;
            Fdata = (Fdata&0x00ff)+ROMReadData;             
          }
          else
          {
            Fdata=ROMReadData;
          }
        }       
       
      }
      WriteConfig_xx.OptionValue[op_addr+WriteConfig_xx.Page] = Fdata;//烧option之前先把值存起来，以方便后续全面校验时读值
      WriteConfig_xx.OptionAddr[op_addr+WriteConfig_xx.Page]=DeviceConfig_xx.OptionAddr+op_addr+option_shift;
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
  
//write checksum
  ReCnt=0;  
  while(++ReCnt<3)
  {
    if(DeviceConfig_xx.MCU_ID==0x3316)
    {
      if(CheckAddr!=0)
      {
        MC321_WriteByte(CheckAddr,CheckSum);
      }
    }
    else if(DeviceConfig_xx.MCU_ID==0x3111)
    {
      MC321_WriteByte(DeviceConfig_xx.OptionAddr+0x0F,CheckSum);
    }    
    else if(DeviceConfig_xx.MCU_ID==0x5312)
    {
      MC321_WriteByte(DeviceConfig_xx.OptionAddr+0x07,CheckSum);
    }
    else if(DeviceConfig_xx.MCU_ID==0x7311)
    {
      MC321_WriteByte(0x8008,CheckSum);
    }
    else if(DeviceConfig_xx.MCU_ID==0x7022)
    {
      if(option_shift==0x00)//
      {
        MC321_WriteByte(0x801D,CheckSum);
      }
      else if(option_shift==0x05)//
      {
        MC321_WriteByte(0x801E,CheckSum);
      }
    }
  }    
//频率验证
  if (DeviceConfig_xx.IRC_Select==0x01)//正式代码
  //if (DeviceConfig_xx.IRC_Select==0x00)//调试代码
  {
    if(DeviceConfig_xx.MCU_ID==0x3316)
    {
      ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+1+option_shift); //set ADDR
      OPTION_FRT=ROMReadData/256;
      OPTION_TEMPADJ=(OPTION_FRT&0xf8);
      OPTION_FRT&=0x07;
      OPTION_FRT2=ROMReadData%256;
      ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+2+option_shift); //set ADDR
      FdataL=ROMReadData%256;
      FdataL&=0x01;
      FdataL<<=5;
      OPTION_TEMPADJ>>=3;
      OPTION_TEMPADJ|=FdataL;
      ReFlag=MC33P116IRC_11Bit_ModeInCheck(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
    }
    else if(DeviceConfig_xx.MCU_ID==0x3111)
    {
      ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+option_shift); //set ADDR
      ROMReadData<<=1;
      OPTION_TEMPADJ=ROMReadData/256;
      OPTION_TEMPADJ&=0x1f;
      ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+1+option_shift); //set ADDR
      OPTION_FRT=ROMReadData/256;
      OPTION_FRT&=0x7ff;
      OPTION_FRT2=ROMReadData%256;
      ReFlag=M31P11IRC_11Bit_ModeInCheck(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
    }
    else if(DeviceConfig_xx.MCU_ID==0x5312)
    {
      ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+1+option_shift); //set ADDR
      OPTION_FRT=ROMReadData/256;
      OPTION_TEMPADJ=(OPTION_FRT&0xf8);
      OPTION_FRT&=0x07;
      OPTION_FRT2=ROMReadData%256;
      ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+2+option_shift); //set ADDR
      FdataL=ROMReadData%256;
      FdataL&=0x01;
      FdataL<<=5;
      OPTION_TEMPADJ>>=3;
      OPTION_TEMPADJ|=FdataL;
      ReFlag=MC32P5312IRC_11Bit_ModeInCheck(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
      ConfigTIM4();//关闭Timer3、Timer4
      if(ReFlag == 0) 
      {
        ERORR_VALUE=IRC_Value_false;
        return 0;
      }
    }
    else if(DeviceConfig_xx.MCU_ID==0x7311)
    {
      ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+1+option_shift); //set ADDR
      OPTION_FRT=ROMReadData/256;
      OPTION_FRT&=0x07;
      OPTION_FRT2=ROMReadData%256;
      ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+2+option_shift); //set ADDR
      TADJ=ROMReadData/256;
      TADJ&=0x3f;
      ReFlag=MC32P7311IRC_10Bit_ModeInCheck(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
      ConfigTIM4();//关闭Timer3、Timer4
      if(ReFlag == 0) 
      {
        ERORR_VALUE=IRC_Value_false;
        return 0;
      }
    }
    else if(DeviceConfig_xx.MCU_ID==0x7022)
    {
      ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+1+option_shift); //set ADDR
      OPTION_FRT=ROMReadData/256;
      OPTION_TEMPADJ=(OPTION_FRT&0xf8);
      OPTION_FRT&=0x07;
      OPTION_FRT2=ROMReadData%256;
      ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+2+option_shift); //set ADDR
      FdataL=ROMReadData%256;
      FdataL&=0x01;
      FdataL<<=5;
      OPTION_TEMPADJ>>=3;
      OPTION_TEMPADJ|=FdataL;
      ReFlag=MC32P7022IRC_11Bit_ModeInCheck(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
      ConfigTIM4();//关闭Timer3、Timer4
      if(ReFlag == 0) 
      {
        ERORR_VALUE=IRC_Value_false;
        return 0;
      }      
    }
    ConfigTIM4();//关闭Timer3、Timer4
    if(ReFlag == 0) 
    {
      ERORR_VALUE=IRC_Value_false;
      return 0;
    }
  }   
//再进模式 
  VDD30V_Off;
  POWER_OFF(vpp00,vdd00);
  ConfigTIM4();
  Delay_1ms(5);
  //power on & model in
  MC321_MODEL_IN(vdd30);
  Delay_1ms(1);
  CLN_VDD2GND_Off;
  VPP12V_On;
//  VPP_IO_On;
  Delay_1ms(10);
  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID)
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  }
  
//rom 三种模式全面读校验
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
      	
        Fdata&=WriteConfig_xx.RomBit;
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
  
//option 三种模式全面读校验
  if(DeviceConfig_xx.WriteEn & 0xf0)
  {
    if(WriteConfig_xx.AllCheckNeed > 0)
    {
      for(op_addr=0;op_addr<DeviceConfig_xx.OptionSize+WriteConfig_xx.Page;op_addr++)
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
  
//write protect bit
  if(DeviceConfig_xx.WriteEn & 0xf0)
  {  
    FM_CS_0;
    FMReadMore(WriteConfig_xx.Addr_Flash_OptionStart+WriteConfig_xx.Page*2+WriteConfig_xx.SecurityOption*2-1);    

    Fdata =Rxdata;
    SPI_ReadByte();//从flash中取一个字节
    FdataL=Rxdata;
    SPI_ReadByte();//再从flash中取一个字节
    FdataH=Rxdata;
    Fdata=FdataH*0x100+FdataL;//组成一个字 
    Fdata&=(0x0001<<WriteConfig_xx.SecurityBit);
    
    ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr+option_shift);
    ROMReadData&=(~(0x0001<<WriteConfig_xx.SecurityBit));
    Fdata|=ROMReadData;


    ReCnt=WriteConfig_xx.ReWriteCnt;
    while(ReCnt--) 
    {	
      ForceReCnt=WriteConfig_xx.ForceWriteCnt;      
      if (Fdata != 0xffff)  // 
      {
        while(ForceReCnt--)
        {
          MC321_WriteByte(DeviceConfig_xx.OptionAddr+WriteConfig_xx.SecurityOption+option_shift,Fdata);
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








