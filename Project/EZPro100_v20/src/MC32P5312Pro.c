
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
#include "MC32P5312Pro.h"

//u8 MC32P5312IRC_11Bit_ModeInCheck(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
//{
//  u8 i;
//  u8 optL,optH,opt_2;
//  u16 mm;
//  u8 OPTION_FAS;
//  u8 OPTION_TEMPADJ_2;
//  
//  FM_CS_0;
//  if (FMReadOne(Addr_Flash_OptionProtec+4)==1)//option 2 ���ֽ�
//  {
//    optL =Rxdata;
//  }
//  else
//  {
//    ERORR_VALUE=FM_Read_false;
//    FM_CS_1;
//    return 0;
//  }
//  if (FMReadOne(Addr_Flash_OptionProtec+5)==1)//option 2 ���ֽ�
//  {
//    optH =Rxdata;
//  }
//  else
//  {
//    ERORR_VALUE=FM_Read_false;
//    FM_CS_1;
//    return 0;
//  }    
//  
//  opt_2 = optL;
//  OPTION_FAS = optH & 0x07; //ȷ����16M 8M 4M 2M 1M 455K��һ�֣���ȷ������У��Ҫ�õ��¶�У׼ֵOPTION_TEMPADJ_1��OPTION_TEMPADJ_2
//   
//  MC301_Instruct(TestMod);
//
//  for(i=0;i<5;i++)
//  {
//    MC321_IRC_INST(0X0000); //5 NOP
//  }
//  if(OPTION_FAS == 0)//16M
//  {
//    IRC_FreqType = 1250;
//    IRC_FreqMin = 1246;
//    IRC_FreqMax = 1254;
//    optL = 0x70;
//  }
//  else if(OPTION_FAS == 1)//8M
//  {
//    IRC_FreqType = 1250;
//    IRC_FreqMin = 1246;
//    IRC_FreqMax = 1254;   
//    optL = 0x60;
//  }
//  else if(OPTION_FAS == 2)//4M
//  {
//    IRC_FreqType = 1250;
//    IRC_FreqMin = 1246;
//    IRC_FreqMax = 1254;    
//    optL = 0x50;
//  }
//  else if(OPTION_FAS == 3)//2M
//  {
//    IRC_FreqType = 1250;
//    IRC_FreqMin = 1246;
//    IRC_FreqMax = 1254;    
//    optL = 0x40;
//  }   
//  else if(OPTION_FAS == 4)//1M
//  {
//    IRC_FreqType = 1250;
//    IRC_FreqMin = 1246;
//    IRC_FreqMax = 1254;    
//    optL = 0x30;
//  }   
//  else if(OPTION_FAS == 5)//455K
//  {
//    IRC_FreqType = 2275;
//    IRC_FreqMin = 2268;
//    IRC_FreqMax = 2282;    
//    optL = 0x10;
//  }   
//  
//  mm=0x3c00+optL;
//  MC321_IRC_INST(mm);
//  MC321_IRC_INST(0x57f9);     //opt0 L
//  
//  optH = (optH & 0x3f) | 0xd8;//opt2 FAS FDS RCSMTB���λ ע�⣺У��ʱ Ҫ��FDSҪд��̶�ֵ11
//  mm=0x3c00+optH;
//  MC321_IRC_INST(mm);
//  MC321_IRC_INST(0x57fe);     //opt2 H
//  
//  optL = (opt_2 & 0x02) | 0xfd;//VSDEL���λ
//  mm=0x3c00+optL;
//  MC321_IRC_INST(mm);
//  MC321_IRC_INST(0x57fd);     //opt2 L
//  
//  OPTION_TEMP = optH;
//  OPTION_TEMP = (OPTION_TEMP<<8)&0xff00;
//  OPTION_TEMP = OPTION_TEMP+optL;
////  Fdata = optH<<8 + optL;
//  
//  MC321_IRC_INST(0x0000) ; 
//  MC321_IRC_INST(0x0000) ;
//
//  MC321_IRC_INST(0x3c01);     //movai 0x01
//  MC321_IRC_INST(0x57f8);     //movra 0x1f8 //set HIRC output to SDO
//
//  MC321_IRC_INST(0x0000) ; 
//  MC321_IRC_INST(0x0000) ;   
//
//  mm=OPTION_FRT*256+OPTION_FRT2;
//  OPTION_TEMPADJ_2=OPTION_TEMPADJ;
//    
//  MC32P5312FTtest_11Bit(mm,OPTION_TEMPADJ_2);
//  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
//  { 
//    return 1;//
//  } 
//  OPTION_FRT=0xff;
//  OPTION_FRT2=0xff;
//  OPTION_TEMPADJ = 0xff;  
//  return 0; 
//}
//
//
//u8 MC32P5312IRC_11Bit_check(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType)
//{
//  u16 mm;
//  u8 OPTION_TEMPADJ_2;
//  u8 optH;
//  
//  if (FMReadOne(Addr_Flash_OptionProtec+5)==1)//option 2 ���ֽ�
//  {
//    optH =Rxdata;
//  }
//  else
//  {
//    ERORR_VALUE=FM_Read_false;
//    FM_CS_1;
//    return 0;
//  }    
//  optH &= 0x07; //ȷ����16M 8M 4M 2M 1M 455K��һ�֣���ȷ������У��Ҫ�õ��¶�У׼ֵOPTION_TEMPADJ_1��OPTION_TEMPADJ_2
//  if(optH == 0)//16M
//  {
//    IRC_FreqType = 1250;
//    IRC_FreqMin = 1246;
//    IRC_FreqMax = 1254;
//  }
//  else if(optH == 1)//8M
//  {
//    IRC_FreqType = 1250;
//    IRC_FreqMin = 1246;
//    IRC_FreqMax = 1254;   
//  }
//  else if(optH == 2)//4M
//  {
//    IRC_FreqType = 1250;
//    IRC_FreqMin = 1246;
//    IRC_FreqMax = 1254;    
//  }
//  else if(optH == 3)//2M
//  {
//    IRC_FreqType = 1250;
//    IRC_FreqMin = 1246;
//    IRC_FreqMax = 1254;    
//  }   
//  else if(optH == 4)//1M
//  {
//    IRC_FreqType = 1250;
//    IRC_FreqMin = 1246;
//    IRC_FreqMax = 1254;    
//  }   
//  else if(optH == 5)//455K
//  {
//    IRC_FreqType = 2275;
//    IRC_FreqMin = 2268;
//    IRC_FreqMax = 2282;    
//  }   
//
//  
//  mm=OPTION_FRT*256+OPTION_FRT2;
//  OPTION_TEMPADJ_2=OPTION_TEMPADJ;
//    
//  MC32P5312FTtest_11Bit(mm,OPTION_TEMPADJ_2);
//  if ((IRC_VALUE<=IRC_FreqMax) && (IRC_VALUE>=IRC_FreqMin))
//  { 
//    return 1;//
//  } 
//  OPTION_FRT=0xff;
//  OPTION_FRT2=0xff;
//  OPTION_TEMPADJ = 0xff;  
//  return 0;  
//}
//
//u8 MC32P5312_Program()
//{
//  u8    i,j,ReCnt=0;
//  u16   Fdata,ROMReadData;
//  u8	FdataL,FdataH;
//  u8    ReFlag=0;
//
//  u8    id_length=0;
//  u16   id_addr0,id_addr1,id_addr2,id_addr3,mcu_addr;
//  u32   id_end,current_id,max_id;
//  
//  u8    ForceReCnt=0;  
//  u8    SuccessCnt;  
//  u8    op_addr=0;
//  u8    option_shift=0;  
//  u16   CheckSum=0,CheckSum_c=0;  
//  u16   Addres;
//  u8    IRC_Empty=0;//
//  
//  WriteConfig_xx.WriteCheckNeed=0;
//  WriteConfig_xx.ReadCnt=1;
//  WriteConfig_xx.ReadOkCnt=1;
//  WriteConfig_xx.ReWriteCnt=3;
//  WriteConfig_xx.ForceWriteCnt=1;
//  WriteConfig_xx.AllCheckNeed=2;   
//  
////�Ƚ�ģʽ  
//  VDD30V_Off;
//  POWER_OFF(vpp00,vdd00);
//  ConfigTIM4();
//  Delay_1ms(5);
//  //power on & model in
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
//  
////IRC adjust
//  if (DeviceConfig_xx.IRC_Select==0x01)//��ʽ����
//  //if (DeviceConfig_xx.IRC_Select==0x00)//���Դ���
//  {
//    ReFlag=MC32P5312IRC_11Bit(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
//    if(ReFlag==1)
//    {
//      ReFlag=MC32P5312IRC_11Bit_check(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
//    }
//    ConfigTIM4();
//    if(ReFlag == 0) 
//    {
//      ERORR_VALUE=IRC_Value_false;
//      return 0;
//    }
//  }      
//  VDD30V_Off;
//  POWER_OFF(vpp00,vdd00);
//  ConfigTIM4();
//  Delay_1ms(5);
//  //power on & model in
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
////Write ROM
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
//      CheckSum_c += FdataL;
//      SPI_ReadByte();
//      FdataH=Rxdata;
//      CheckSum +=FdataH;
//      CheckSum_c += (FdataH<<8);
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
//        if (DeviceConfig_xx.MCU_ID==0x5312)
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
////Write OPTION  
//  if(DeviceConfig_xx.WriteEn & 0xf0)
//  {
//    FM_CS_0;
//    FMReadMore(Addr_Flash_Option-1);
//    Fdata =Rxdata;
//    
//    ROMReadData=MC321_ReadByte(DeviceConfig_xx.OptionAddr); //set ADDR
//    for(op_addr=0;op_addr<DeviceConfig_xx.OptionSize;op_addr++)//
//    {
//      SPI_ReadByte();//��flash��ȡһ���ֽ�
//      FdataL=Rxdata;
//      SPI_ReadByte();//�ٴ�flash��ȡһ���ֽ�
//      FdataH=Rxdata;
//      Fdata=FdataH*0x100+FdataL;//���һ����
//      
//      //option0 
//      if(op_addr==0)
//      {          
//        Fdata|=0x8000;//��д����λ
//      }      
//      //option1 //Ƶ��У׼ֵ �� �¶�У׼ֵ  
//      if(op_addr==1)
//      {          
//        ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);
//        Fdata = ROMReadData;
//        if(ROMReadData==0xffff)//
//        {
//          ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift+1);
//          if((ROMReadData&0x0001)==0x0001)//
//          {
//            IRC_Empty=1;
//            FdataH =OPTION_FRT; //IRC from QC test
//            FdataL =OPTION_FRT2;
//            FdataH = (OPTION_TEMPADJ<<3)&0xf8 | OPTION_FRT ;      
//            Fdata=FdataH*0x100+FdataL;
//          }
//        }
//      }
//      else if(op_addr==2)//�¶�У׼ֵ  
//      {
//        ROMReadData = MC321_ReadByte(DeviceConfig_xx.OptionAddr+op_addr+option_shift);
//        Fdata=(Fdata&0xfffe)|(ROMReadData&0x0001);
//        if(IRC_Empty)//
//        {
//          FdataL = (FdataL&0xfe) | ((OPTION_TEMPADJ&0x20)>>5);//OPTION2 �¶�У׼λbit6
//          Fdata=FdataH*0x100+FdataL;
//        }
//      }
//
//      WriteConfig_xx.OptionValue[op_addr] = Fdata;//��option֮ǰ�Ȱ�ֵ���������Է������ȫ��У��ʱ��ֵ
//      WriteConfig_xx.OptionAddr[op_addr]=DeviceConfig_xx.OptionAddr+op_addr+option_shift;
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
////write checksum
//  ReCnt=0;  
//  while(++ReCnt<3)
//  {
//    MC321_WriteByte(DeviceConfig_xx.OptionAddr+0x07,CheckSum);
//  }   
//  
////Ƶ����֤ ȡд��оƬ���¶�У׼ֵ��Ƶ��У׼ֵ
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
//    ReFlag=MC32P5312IRC_11Bit_ModeInCheck(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);
//    ConfigTIM4();//�ر�Timer3��Timer4
//    if(ReFlag == 0) 
//    {
//      ERORR_VALUE=IRC_Value_false;
//      return 0;
//    }
//  }   
//  
////�ٽ�ģʽ 
//  VDD30V_Off;
//  POWER_OFF(vpp00,vdd00);
//  ConfigTIM4();
//  Delay_1ms(5);
//  //power on & model in
//  MC321_MODEL_IN(vdd30);
//  Delay_1ms(1);
//  CLN_VDD2GND_Off;
//  VPP12V_On;
////  VPP_IO_On;
//  Delay_1ms(10);
//  Fdata=MC321_ReadByte(0XFFFF);
//  if (Fdata !=DeviceConfig_xx.MCU_ID)
//  {
//    ERORR_VALUE=OTP_ModeIn_false;
//    return 0;
//  } 
//  
////rom ����ģʽȫ���У��
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
////option ����ģʽȫ���У��
//  if(DeviceConfig_xx.WriteEn & 0xf0)
//  {
//    if(WriteConfig_xx.AllCheckNeed > 0)
//    {
//      for(op_addr=0;op_addr<DeviceConfig_xx.OptionSize;op_addr++)
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
//  if(DeviceConfig_xx.WriteEn & 0xf0)
//  {  
//    FM_CS_0;
//    FMReadMore(Addr_Flash_Option-1);
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
//
//
//
//
//


