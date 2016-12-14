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
  VDD65V_Off;//��Ҫ�������ֵ�λ�����ڳ�����5V,��U10������6.5V�ص�  
  POWER_OFF(vpp00,vdd00);
  //�ر�ע�� Delay_1ms������ʱ���Ҫ����STM32оƬ��ʱ������������
  Delay_1ms(100);//�ϵ����� ���ʱ���Գ�һ��
  OTP_SCK_0;
  OTP_SDI_0; 

  //��оƬ����
  if (VddFlag == vdd50)//
  {
    VDD_On;
    IO5V_On;//�鿴�����ֲ��֪6060������ѹ����Ϊ5V
  }
  CLN_VDD2GND_Off;//�ض�Q5�����Q14��ͨ��D8��ͨ��VPP������VDD,9033����ģʽ��ʱ����Ҫ������6060��ģʽ����ҪVPPʱ��,����VPPʱ���ض�Q14
  Delay_100us(5);
  Delay_1ms(1);
  NOP;

  OTP_SCK_1;//SDI��1
  OTP_SDI_1;
  NOP;
  OTP_SCK_0;
  NOP;
  
  OTP_SCK_1;//SDI��0
  OTP_SDI_0;
  NOP;
  OTP_SCK_0;
  NOP;

  OTP_SCK_1;//SDI��1
  OTP_SDI_1;
  NOP;
  OTP_SCK_0;
  NOP;
  
  OTP_SCK_1;//SDI��1
//  OTP_SDI_1;
  NOP;
  OTP_SCK_0;
  NOP;
  
  OTP_SCK_1;//SDI��0
  OTP_SDI_0;
  NOP;
  OTP_SCK_0;
  NOP;
  
  OTP_SCK_1;//SDI��1
  OTP_SDI_1;
  NOP;
  OTP_SCK_0;
  NOP;
  
  OTP_SCK_1;//SDI��1
//  OTP_SDI_1;
  NOP;
  OTP_SCK_0;
  NOP;
  
  OTP_SCK_1;//SDI��1
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
* Description    : ȫ����ѡ��ĳһ����в���
* Input          : OTPData 0x20:ȫ��Ϊ0��0x30ȫ��Ϊ1��0x20+num������num��Ϊ0��0x30+num������num��Ϊ1
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
  return 0;//�����ɹ�
}
void MC32E22FTtest(u8 IRC_Option)
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
    MC32E22FTtest(mm);
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
    return 1;//У׼�ɹ� 
  } 
  OPTION_FRT=0XFF;
  return 0;//У׼ʧ�� 
}
/*******************************************************************************
* Function Name  : MC32E22_Program
* Description    : MC32E22�в������ܣ����жϱ��β����ǲ��ǲ���������ǲ�������
*                  ֻ���в�������������д���򣬾Ͳ����в���������ֻ��д
* Input          : None
* Output         : None
* Return         : 1 :�ɹ�  0��ʧ��
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

 //-------------------------���²�����ش���-----------------------
/*
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
  
  //power off then power on & model in
  MC32E22_MODEL_IN(vdd50);
  Fdata=MC321_ReadByte(0XFFFF);
  if (Fdata !=DeviceConfig_xx.MCU_ID) 
  {
    ERORR_VALUE=OTP_ModeIn_false;
    return 0;
  }
  temp = erase_en & 0x0006;
  if(temp == 0x00 || temp == 0x06)//��Ҫ����
  {//��PCͨѶЭ���� ��00-��������̣�01-ֻ��̣�11-ֻ����
    temp = (((u8)erase_en) & 0x01)<<4;//��Ϊ1��0
    if(erase_num == 0x00)//ȫ�� ������δ��ס�Ŀ�
    {
      ReFlag = MC32E22_ERASE(0x20+temp);//ȫ��Ϊ1��0
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
          ReFlag = MC32E22_ERASE(0x00+temp+i);//���Ϊ1��0
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
  */
//-------------------------����У����ش���----------------------- 
//  if (DeviceConfig_xx.IRC_Select==0x01)//��ʽ����
  if (DeviceConfig_xx.IRC_Select==0x00)//���Դ���
  {
    ReFlag=MC32E22IRC(DeviceConfig_xx.IRC_FrMin,DeviceConfig_xx.IRC_FrMax,DeviceConfig_xx.IRC_FrType);   
    if(ReFlag == 0) 
    {
      ERORR_VALUE=IRC_Value_false;
      return 0;
    }
  }
//-------------------------����дROM��ش���----------------------- 
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
//  if(DeviceConfig_xx.WriteEn == 0)//����
  {
    CheckSum=0;
    FM_CS_0;
    FMReadMore(Addr_Flash_ROMStart+DeviceConfig_xx.RomFirAddr-1);
    Fdata =Rxdata;
    
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
      
      Fdata = 0x55aa;
      
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
      //�����ж��Ƿ�д��
      if (OTP_ADDR<DeviceConfig_xx.RomEndAddr)
      {//û��д�� rom��ַ��1 ����д
        MC301_Instruct(AddrINC);
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

      //option0 //
      /*
      if(op_addr == 0)
      {
        Fdata = 0xfcf0;//���ڲ���Ƶ
      }*/
      //option1 
      if(op_addr==1)
      {
        Fdata = (Fdata&0xff00) + OPTION_FRT;       //У׼�õ���Ƶ��У׼ֵ
      }
      //option2                 //���ݲ��Խ�� ��ʽ��Ʒ��Ҫд��                                                
      if(op_addr==2)//��ѹѡ��λ �¶�У׼λ ���Խ׶� ������λ��д  
      {
      //  Fdata=0xBF;//���HIRC��ѹѡ��λѡ1.9V������0xBF
      //  Fdata=0xAF;//���HIRC��ѹѡ��λѡ1.7V������0xAF        
      }
      //option3 //����Ҫ��
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