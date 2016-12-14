
#include "stm32f10x.h"

#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"

#include "stm32f10x_tim.h"
#include "global.h"
#include "spi.h"
#include "power.h"
#include "OTPRW.h"
#include "IRC.h"
#include "M201IRC.h"
#include "MC224BPro.h"
#include "Config.h"
#include "LCD.h"

// LCD initial funtion
void displayfuntion()
{
	//Initial_Lcd();
	//128*64  8*16
	u16 i,a,b;
	clear_screen();    //clear all dots
	//displayAZ(lcd_a,0x02,0x03,0x09);
	displayCHAR(lcd_xing,0x00,0x01,01); //16*16
	displayCHAR(lcd_hao,0x00,0x01,17);
	displayAZ(lcd_maohao,0x00,0x01,33);
	a=41;
	b=0;
	for (i=0;i<10;i++)
	{
		if ((DeviceConfig_xx.MCU_Name[i]>0x29) && (DeviceConfig_xx.MCU_Name[i]<0x7b))
		{
			b=8*i;
			a=41+b;
			displayAZ(pp[DeviceConfig_xx.MCU_Name[i]-0x30],0x00,0x01,a); //MCXXXXXX
		}
	}
	//CheckSum Option
	//USB_RxDataCS=CalculateCheckSum();
	display_CheckSum_Option();
	//19,20,21,22
	if (iic_data[0]==0x0a)
	{
		OKcounter=(iic_data[19]<<24)+(iic_data[20]<<16)+(iic_data[21]<<8)+iic_data[22];
		display_dynamicID();
	}
	else
	{
		OKcounter=0;
	}

	display_OK_NG(OKcounter,1);
	display_OK_NG(0,0);

	//displaWrite_Check();
   
	//displaygraphic(graphic2);  //display a picture of 128*64 dots
}

void displayTestMode()
{
	clear_screen(); 

	displayAZ(lcd_T,0x00,0x01,1);
	displayAZ(lcd_e,0x00,0x01,9);
	displayAZ(lcd_s,0x00,0x01,17);
	displayAZ(lcd_t,0x00,0x01,25);
	displayAZ(lcd_M,0x00,0x01,33);
	displayAZ(lcd_o,0x00,0x01,41);
	displayAZ(lcd_d,0x00,0x01,49);
	displayAZ(lcd_e,0x00,0x01,57);
	displayAZ(lcd_1,0x00,0x01,65);
	displayAZ(lcd_3,0x00,0x01,73);
	displayAZ(lcd_point,0x00,0x01,81);
	displayAZ(lcd_0,0x00,0x01,89);
	displayAZ(lcd_1,0x00,0x01,97);
	displayAZ(lcd_point,0x00,0x01,105);
	displayAZ(lcd_3,0x00,0x01,113);
	displayAZ(lcd_1,0x00,0x01,121);

}
void displayVDD_VPP(u32 vdd,u32 vpp)
{
	//CheckSum Option
	u32 a,b;

	displayAZ(lcd_V,0X02,0X03,1);
	displayAZ(lcd_D,0X02,0X03,9);
	displayAZ(lcd_D,0x02,0x03,17);

	 a=vdd/100000; //121392
	 b=vdd%100000;

	displayAZ(lcd_maohao,0X02,0X03,25);
	displayAZ(pNum[a],0X02,0X03,32);

	a=b/10000;
	b=b%10000;
	displayAZ(pNum[a],0X02,0X03,41);

	displayAZ(lcd_point,0X02,0X03,49);
	a=b/1000;
	vdd=b%1000;
	b=vdd/100;
	displayAZ(pNum[a],0X02,0X03,57);
	displayAZ(pNum[b],0X02,0X03,65);
	displayAZ(lcd_V,0X02,0X03,73);

	//vpp
	a=vpp/100000;
	b=vpp%100000;
	displayAZ(lcd_V,0x04,0x05,1);
	displayAZ(lcd_P,0x04,0x05,9);
	displayAZ(lcd_P,0x04,0x05,17);
	displayAZ(lcd_maohao,0X04,0X05,25);
	displayAZ(pp[a],0X04,0X05,33);
	a=b/10000;
	b=b%10000;
	displayAZ(pp[a],0X04,0X05,41);

	displayAZ(lcd_point,0X04,0X05,49);
	a=b/1000;
	vpp=b%1000;
	b=vpp/100;
	displayAZ(pp[a],0X04,0X05,57);
	displayAZ(pp[b],0X04,0X05,65);
	displayAZ(lcd_V,0X04,0X05,73);

}

void displayTestState(u8 stateNum)
{
	displayAZ(lcd_blank,0X06,0X07,1);
	displayAZ(lcd_blank,0X06,0X07,9);
	displayAZ(lcd_T,0X06,0X07,17);
	displayAZ(lcd_e,0X06,0X07,25);
	displayAZ(lcd_s,0X06,0X07,33);
	displayAZ(lcd_t,0X06,0X07,41);
	displayAZ(lcd_N,0X06,0X07,49);
	displayAZ(lcd_u,0X06,0X07,57);
	displayAZ(lcd_m,0X06,0X07,65);
	displayAZ(lcd_maohao,0X06,0X07,73); 
	displayAZ(pNum[stateNum],0X06,0X07,81);//16����
	//displayAZ(lcd_y,0X06,0X07,89);
	//displayAZ(lcd_point,0X06,0X07,97);
	//displayAZ(lcd_point,0X06,0X07,105);
	//displayAZ(lcd_point,0X06,0X07,113);
	//displayAZ(lcd_point,0X06,0X07,121);
}

void display_CheckSum_Option()
{
	//CheckSum Option
	u16 a,b;
	//u8 p0,p1,p2,p3;

	displayAZ(lcd_C,0X02,0X03,1);
	displayAZ(lcd_S,0X02,0X03,9);
	displayAZ(lcd_maohao,0x02,0x03,17);
	//USB_RxDataCS=CalculateCheckSum();

	a=USB_RxDataCS/0x1000; //x0axxx;
	b=USB_RxDataCS%0x1000;

	//displayAZ(lcd_0,0X02,0X03,25);
	//displayAZ(lcd_x,0X02,0X03,32);
	displayAZ(pNum[a],0X02,0X03,25);
	a=b/0x100;
	b=b%0x100;
	displayAZ(pNum[a],0X02,0X03,32);
	a=b/0x10;
	b=b%0x10;
	displayAZ(pNum[a],0X02,0X03,41); //57
	displayAZ(pNum[b],0X02,0X03,49); //65

	displayAZ(lcd_blank,0X02,0X03,57);

	//otption value
	if ((DeviceConfig_xx.MCU_ID==0x0238)||(DeviceConfig_xx.MCU_ID==0x3378)||(DeviceConfig_xx.MCU_ID==0x3394)
		||(DeviceConfig_xx.MCU_ID==0x7803)||(DeviceConfig_xx.MCU_ID==0x6803)) 
	{
		a=FMReadOne(Addr_Flash_OptionProtec8k);
		a=Rxdata;
		b=a/0x10;
		a=a%0x10;
		displayAZ(pNum[b],0X02,0X03,65);
		displayAZ(pNum[a],0X02,0X03,73);

		a=FMReadOne(Addr_Flash_OptionProtec8k+1);
		a=Rxdata;
		b=a/0x10;
		a=a%0x10;
		displayAZ(pNum[b],0X02,0X03,81);
		displayAZ(pNum[a],0X02,0X03,89);
		a=FMReadOne(Addr_Flash_OptionProtec8k+2);
		a=Rxdata;
		b=a/0x10;
		a=a%0x10;
		displayAZ(pNum[b],0X02,0X03,97);
		displayAZ(pNum[a],0X02,0X03,105);
		a=FMReadOne(Addr_Flash_OptionProtec8k+3);
		a=Rxdata;
		b=a/0x10;
		a=a%0x10;
		displayAZ(pNum[b],0X02,0X03,113);
		displayAZ(pNum[a],0X02,0X03,121);
	} 
	else if(DeviceConfig_xx.MCU_ID==0x3316)
        {
		a=FMReadOne(addr_Flash_OptionProtec16K);
		a=Rxdata;
		b=a/0x10;
		a=a%0x10;
		displayAZ(pNum[b],0X02,0X03,65);
		displayAZ(pNum[a],0X02,0X03,73);

		a=FMReadOne(addr_Flash_OptionProtec16K+1);
		a=Rxdata;
		b=a/0x10;
		a=a%0x10;
		displayAZ(pNum[b],0X02,0X03,81);
		displayAZ(pNum[a],0X02,0X03,89);
		a=FMReadOne(addr_Flash_OptionProtec16K+2);
		a=Rxdata;
		b=a/0x10;
		a=a%0x10;
		displayAZ(pNum[b],0X02,0X03,97);
		displayAZ(pNum[a],0X02,0X03,105);
		a=FMReadOne(addr_Flash_OptionProtec16K+3);
		a=Rxdata;
		b=a/0x10;
		a=a%0x10;
		displayAZ(pNum[b],0X02,0X03,113);
		displayAZ(pNum[a],0X02,0X03,121);          
        }
        else
	{
		a=FMReadOne(Addr_Flash_OptionProtec);
		a=Rxdata;
		b=a/0x10;
		a=a%0x10;
		displayAZ(pNum[b],0X02,0X03,65);
		displayAZ(pNum[a],0X02,0X03,73);

		a=FMReadOne(Addr_Flash_OptionProtec+1);
		a=Rxdata;
		b=a/0x10;
		a=a%0x10;
		displayAZ(pNum[b],0X02,0X03,81);
		displayAZ(pNum[a],0X02,0X03,89);
		a=FMReadOne(Addr_Flash_OptionProtec+2);
		a=Rxdata;
		b=a/0x10;
		a=a%0x10;
		displayAZ(pNum[b],0X02,0X03,97);
		displayAZ(pNum[a],0X02,0X03,105);
		a=FMReadOne(Addr_Flash_OptionProtec+3);
		a=Rxdata;
		b=a/0x10;
		a=a%0x10;
		displayAZ(pNum[b],0X02,0X03,113);
		displayAZ(pNum[a],0X02,0X03,121);
	}

}

void displaWrite_Check()
{
	displayAZ(lcd_W,0X06,0X07,1);
	displayAZ(lcd_r,0X06,0X07,9);
	displayAZ(lcd_i,0X06,0X07,17);
	displayAZ(lcd_t,0X06,0X07,25);
	displayAZ(lcd_e,0X06,0X07,33);
	displayAZ(lcd_henggan,0X06,0X07,41);
	displayAZ(lcd_V,0X06,0X07,49);
	displayAZ(lcd_e,0X06,0X07,57);
	displayAZ(lcd_r,0X06,0X07,65);
	displayAZ(lcd_i,0X06,0X07,73);
	displayAZ(lcd_f,0X06,0X07,81);
	displayAZ(lcd_y,0X06,0X07,89);
	displayAZ(lcd_point,0X06,0X07,97);
	displayAZ(lcd_point,0X06,0X07,105);
	displayAZ(lcd_point,0X06,0X07,113);
	displayAZ(lcd_point,0X06,0X07,121);


}

void display_OK_NG(u32 okcounter,u8 ok_ng_flag)
{
	u8 g[6],i;
	u8 shift;
	
	for (i=0;i<6;i++)
	{
		g[i]=okcounter%10;
		okcounter=okcounter/10;
	}
    
	shift=0;
	if (ok_ng_flag==1)
	{
		//4000k-4060kHz
		displayAZ(lcd_O,0x04,0x05,1);
		displayAZ(lcd_K,0x04,0x05,9);
		displayAZ(lcd_maohao,0x04,0x05,17);
			
		for (i=5;i>0;i--)
		{
			displayAZ(pp[g[i]],0X04,0X05,25+shift);
			shift=shift+8;
		}
		displayAZ(pp[g[0]],0X04,0X05,25+shift);
	}
	else
	{
		displayAZ(lcd_N,0X04,0X05,78);
		displayAZ(lcd_G,0X04,0X05,86);
		displayAZ(lcd_maohao,0x04,0x05,92);
		
		for (i=2;i>0;i--)
		{
			displayAZ(pp[g[i]],0X04,0X05,100+shift);
			shift=shift+8;
		}
		displayAZ(pp[g[0]],0X04,0X05,100+shift);
	}

}

void displayErorr(u8 erorrNum)
{

	u8 g,s,b;
	//u8 temp;

	b=erorrNum/100;
	erorrNum=erorrNum%100;
	s=erorrNum/10;
	g=erorrNum%10; 

	displayCHAR(lcd_cuo,4,5,1);
	displayCHAR(lcd_wu,4,5,17);
	displayCHAR(lcd_dai,4,5,33);
	displayCHAR(lcd_ma,4,5,49);
	displayAZ(lcd_maohao,4,5,65);

	displayAZ(lcd_blank,4,5,73);
	displayAZ(lcd_blank,4,5,81);

	displayAZ(pp[b],0X04,0X05,89);
	displayAZ(pp[s],0X04,0X05,97);
	displayAZ(pp[g],0X04,0X05,105);

	displayAZ(lcd_blank,4,5,113);
	displayAZ(lcd_blank,4,5,121);

}

void display_dynamicID()
{
	u32 id_lcd_current=0;
	//u32 temp=0;
	u8 i;
	u8	g[10];
	u8 shift=0;

	if (iic_data[0]!=0x0a)
	{
		return ;
	}

    id_lcd_current=(iic_data[10]<<24)+(iic_data[11]<<16)+(iic_data[12]<<8)+iic_data[13];

	for (i=0;i<10;i++)
	{
		g[i]=id_lcd_current%0x10; //
		id_lcd_current=id_lcd_current/0x10;
		//id_lcd_current=temp;
		if (id_lcd_current==0)
		{
			break;
		}
	}
	
	displayAZ(lcd_I,0x06,0x07,1); //ID:
    displayAZ(lcd_D,0x06,0x07,9);
	displayAZ(lcd_maohao,0X06,0X07,17);

	displayAZ(lcd_0,0X06,0X07,25);
	displayAZ(lcd_x,0X06,0X07,33);

	shift=0;
	for ( ;i>0;i--)
	{
		displayAZ(pNum[g[i]],0X06,0X07,41+shift);
		shift+=8;
	}

	displayAZ(pNum[g[0]],0X06,0X07,41+shift);
	
}

void display_Addr()
{
	u16 a,b;
	displayAZ(lcd_A,0X06,0X07,1); //Addr:
	displayAZ(lcd_d,0X06,0X07,9);
	//displayAZ(lcd_d,0X06,0X07,17);
	displayAZ(lcd_r,0X06,0X07,17);
	displayAZ(lcd_maohao,0X06,0X07,25);

	a=OTP_ADDR/0x1000; //x0axxx;
	b=OTP_ADDR%0x1000;

	displayAZ(lcd_0,0X06,0X07,33);
	displayAZ(lcd_x,0X06,0X07,41);
	displayAZ(pNum[a],0X06,0X07,49);
	a=b/0x100;
	b=b%0x100;
	displayAZ(pNum[a],0X06,0X07,57);
	a=b/0x10;
	b=b%0x10;
	displayAZ(pNum[a],0X06,0X07,65);
	displayAZ(pNum[b],0X06,0X07,73);

	//0xss -ss
	//USB_Tx_Buffer[6]=0xaa; //Rxdata;  //Flash data
	//USB_Tx_Buffer[7]=OTP_ReadData;
    displayAZ(lcd_blank,0X06,0X07,81);
	a=Rxdata/0x10;
	b=Rxdata%0x10;
	displayAZ(pNum[a],0X06,0X07,89);
	displayAZ(pNum[b],0X06,0X07,97);
	displayAZ(lcd_henggan,0x06,0x07,105);
	a=OTP_ReadData/0x10;
	b=OTP_ReadData%0x10;
	displayAZ(pNum[a],0X06,0X07,113);
	displayAZ(pNum[b],0X06,0X07,121);



}

void displayTestResult(int FreqValue,u8 page0,u8 page1,u8 lineNumb)
{
	u8 g,s,b,q,w;
	int temp;

	w=FreqValue/10000;
	temp=FreqValue%10000;
	FreqValue=temp;
	q=FreqValue/1000;
	temp=FreqValue%1000;
	b=temp/100;
	FreqValue=temp%100;;
	s=FreqValue/10;
	g=FreqValue%10;

	displayAZ(pp[w],page0,page1,lineNumb);
	displayAZ(pp[q],page0,page1,lineNumb+8);
	displayAZ(pp[b],page0,page1,lineNumb+16);
	displayAZ(pp[s],page0,page1,lineNumb+24);
	displayAZ(pp[g],page0,page1,lineNumb+32);
}

const u8 *pp[]={
  //0x30---0x39,
  lcd_0,lcd_1,lcd_2,lcd_3,lcd_4,lcd_5,lcd_6,lcd_7,lcd_8,lcd_9,lcd_9 ,lcd_9 ,lcd_9 ,lcd_9 ,lcd_9 ,lcd_9 ,lcd_9 ,
  //0x41----0x5a
  lcd_A,lcd_B,lcd_C,lcd_D,lcd_E,lcd_F,lcd_G,lcd_H1,lcd_I,lcd_J,lcd_K,
  lcd_L,lcd_M,lcd_N,lcd_O,lcd_P,lcd_Q,lcd_R,lcd_S,lcd_T,lcd_U,lcd_V,
  lcd_W,lcd_X,lcd_Y,lcd_Z,lcd_Z,lcd_Z,lcd_Z,lcd_Z,lcd_Z,lcd_Z,
  //0x61----0x7a
  lcd_a,lcd_b,lcd_c,lcd_d,lcd_e,lcd_f,lcd_g,lcd_h1,lcd_i,lcd_j,lcd_k,
  lcd_l,lcd_m,lcd_n,lcd_o,lcd_p,lcd_q,lcd_r,lcd_s,lcd_t,lcd_u,lcd_v,
  lcd_w,lcd_x,lcd_y,lcd_z

};
const u8 *pNum[]={
	//0x00---0x0f,
	lcd_0,lcd_1,lcd_2,lcd_3,lcd_4,lcd_5,lcd_6,lcd_7,lcd_8,lcd_9,
	lcd_A,lcd_B,lcd_C,lcd_D,lcd_E,lcd_F,
};
/*��ʼ���ӳ���*/
void Initial_Lcd()
{
        u8 i;
	GPIO_InitTypeDef GPIO_InitStructure;

	//PIN39_RES ����λ��L��LCD��λ���ص��ߵ�ƽ��Һ��ģ�鿪ʼ����
	//PIN40_SDA
	//PIN41_SCK
	//PIN42_RS:�Ĵ���ѡ���źţ�H�����ݣ�L:ָ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_8|GPIO_Pin_10|GPIO_Pin_11;

	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;// GPIO_Mode_Out_OD; // ��©��� 
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_Init(GPIOE, &GPIO_InitStructure);

	LCD_RST_1;
	LCD_SCK_1;
	Delay_100us(1);
        //reset=0;                 //Reset the chip when reset=0 
		LCD_RST_0;
        //delay(20);
		Delay_1ms(1);
        //reset=1;
		LCD_RST_1;
		DELAY_14US;
	transfer_command(0xe2);		/*����λ*/
	transfer_command(0x2c);		/*��ѹ����1*/
	DELAY_14US;//Delay_100us(1); 
	transfer_command(0x2e);		/*��ѹ����2*/
	DELAY_14US;//Delay_100us(1);
	transfer_command(0x2f);		/*��ѹ����3*/
	DELAY_14US;//Delay_100us(1);
	transfer_command(0x23);		/*�ֵ��Աȶȣ������÷�Χ20��27*/
	transfer_command(0x81);		/*΢���Աȶ�*/
	transfer_command(0x27);		/*΢���Աȶȵ�ֵ�������÷�Χ0��63*/
	transfer_command(0xa2);		/*1/9ƫѹ�ȣ�bias��*/
	transfer_command(0xc8);		/*��ɨ��˳�򣺴��ϵ���*/
	transfer_command(0xa0);		/*��ɨ��˳�򣺴�����*/
	transfer_command(0x60);		/*��ʼ�У��ӵ�һ�п�ʼ*/
	transfer_command(0xaf);		/*����ʾ*/


}



//===============clear all dot martrics=============
void clear_screen()
{
   u8 i,j;
       
 	for(i=0;i<9;i++)
        {
        	//cs1=0;
		
		transfer_command(0xb0+i);
		transfer_command(0x10);
		transfer_command(0x01);
		for(j=0;j<128;j++)
		{
	        	transfer_data(0x00);
		}
         }
}
void clear_line(u8 page0)
{
	u8 i,j;

	for(i=0;i<2;i++)
	{
		//cs1=0;

		transfer_command(0xb0+i+page0);
		transfer_command(0x10);
		transfer_command(0x01);
		for(j=0;j<128;j++)
		{
			transfer_data(0x00);
		}
	}
}

//=================display a~~z 16*8 dots ========================
void displayAZ(const u8 *dp,u8 page0,u8 page1,u8 lineNum)
{
     u8 i;
	 u8 lineNumH,lineNumL;
	 lineNumH=lineNum/16;
	 lineNumL=lineNum%16;

     transfer_command(0xb0+page0);
	 transfer_command(0x10+lineNumH);
	 transfer_command(lineNumL);
	 for (i=0;i<8;i++)
	 {
		 transfer_data(*dp);
		 dp++;
	 }
	 transfer_command(0xb0+page1);
	 transfer_command(0x10+lineNumH);
	 transfer_command(lineNumL);
	 for (i=0;i<8;i++)
	 {
		 transfer_data(*dp);
		 dp++;
	 }
	 
  
}

//==============display char ================================
void displayCHAR(const u8 *dp,u8 page0,u8 page1,u8 lineNum)
{
	u8 i;
	u8 lineNumH,lineNumL;
	lineNumH=lineNum/16;
	lineNumL=lineNum%16;

	transfer_command(0xb0+page0);
	transfer_command(0x10+lineNumH);
	transfer_command(lineNumL);
	for (i=0;i<16;i++)
	{
		transfer_data(*dp);
		dp++;
	}
	transfer_command(0xb0+page1);
	transfer_command(0x10+lineNumH);
	transfer_command(lineNumL);
	for (i=0;i<16;i++)
	{
		transfer_data(*dp);
		dp++;
	}
}



//==================display a piture of 128*64 dots================
void displaygraphic(const u8  *dp)
{
        int i,j;
       	for(i=0;i<2;i++)
        {
        	//cs1=0;
		transfer_command(0xb0+i);   //set page address,
		transfer_command(0x10); //line H 0001_xxxxb
		transfer_command(0x01); //line L 0000_xxxxb the first num is 01
		for(j=0;j<255;j++)
		{
	        	transfer_data(*dp);    
			dp++; 
                        if(j>0)
                        {
                          if((j<255)&((j%16)==0))
                          {
                            j=j+16;
                            dp=dp+16;
                          }
                        }

		}
                dp=dp-241;
          }
 	
}

/*=========дָ��===============*/
void transfer_command( u8 data1)   
{
	u8 i;
	//cs1=0;
	//rs=0;
	LCD_RS_0;
	DELAY_14US;
	for(i=0;i<8;i++)
	{
		//sclk=0;
		LCD_SCK_0;
	    if(data1&0x80) LCD_SDA_1;//sid=1;
	    else LCD_SDA_0;//sid=0;
		//delay1(1);
		Delay_100us(1);
		//sclk=1;
		LCD_SCK_1;
		Delay_100us(1);
	 	data1=data1<<1;
 		
	  }
	  LCD_SCK_1;

}

/*-------д����---------------*/
void transfer_data(u8 data1)
{
	u8 i;
	//cs1=0;
	//rs=1;
	LCD_RS_1;
	DELAY_14US;
	for(i=0;i<8;i++)
	       {
		//sclk=0;
		LCD_SCK_0;
		if(data1&0x80) LCD_SDA_1;//sid=1;
		else LCD_SDA_0;//sid=0;
		LCD_SCK_1;
		//sclk=1;		
	 	data1=data1<<1; 		
	        }
	LCD_SCK_1;

}


//const u8  graphic2[]={
//	/*--  ����:  ��  --*/
//	/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=16x16   --*/
//	0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x00,0x00,
//	0x40,0x40,0x40,0x40,0x40,0x40,0x7F,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,
//
//	/*--  ����:  ��  --*/
//	/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=16x16   --*/
//	0x10,0x60,0x02,0x0C,0xC0,0x10,0x08,0xF7,0x14,0x54,0x94,0x14,0xF4,0x04,0x00,0x00,
//	0x04,0x04,0x7C,0x03,0x00,0x01,0x1D,0x13,0x11,0x55,0x99,0x51,0x3F,0x11,0x01,0x00,
//
//	/*--  ����:  ��  --*/
//	/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=16x16   --*/
//	0x00,0x00,0xC0,0x5F,0x55,0x55,0x55,0x55,0xF5,0x55,0x55,0x5F,0x60,0x40,0x00,0x00,
//	0x80,0x60,0x1F,0x02,0x12,0xA2,0x9E,0x40,0x43,0x2C,0x10,0x2C,0x43,0xF0,0x00,0x00,
//
//	/*--  ����:  ��  --*/
//	/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=16x16   --*/
//	0x04,0x84,0xE4,0x5C,0x44,0xC4,0x80,0x40,0x30,0x4F,0x88,0x08,0x08,0x88,0x78,0x00,
//	0x02,0x01,0x7F,0x10,0x10,0x3F,0x80,0x80,0x40,0x20,0x10,0x09,0x06,0x01,0x00,0x00,
//
//	/*--  ����:  ΢  --*/
//	/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=16x16   --*/
//	0x10,0x88,0xC4,0x33,0x80,0x9E,0x90,0x9F,0x90,0x9E,0x20,0xD8,0x17,0xF0,0x10,0x00,
//	0x01,0x00,0xFF,0x80,0x40,0x3E,0x02,0x02,0x3E,0x10,0x88,0x67,0x18,0x67,0x80,0x00,
//
//	/*--  ����:  ��  --*/
//	/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=16x16   --*/
//	0x00,0x00,0xF8,0x88,0x88,0x88,0x88,0xFF,0x88,0x88,0x88,0x88,0xF8,0x00,0x00,0x00,
//	0x00,0x00,0x1F,0x08,0x08,0x08,0x08,0x7F,0x88,0x88,0x88,0x88,0x9F,0x80,0xF0,0x00,
//
//	/*--  ����:  ��  --*/
//	/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=16x16   --*/
//	0x80,0x82,0x82,0x82,0x82,0x82,0x82,0xE2,0xA2,0x92,0x8A,0x86,0x82,0x80,0x80,0x00,
//	0x00,0x00,0x00,0x00,0x00,0x40,0x80,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//
//	/*--  ����:  ��  --*/
//	/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=16x16   --*/
//	0x04,0x04,0x04,0x84,0xE4,0x3C,0x27,0x24,0x24,0x24,0x24,0xE4,0x04,0x04,0x04,0x00,
//	0x04,0x02,0x01,0x00,0xFF,0x09,0x09,0x09,0x09,0x49,0x89,0x7F,0x00,0x00,0x00,0x00
//
//
//};


//const u8 lcd_gong[]={
///*--  ����:  ��  --*/
///*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=16x16   --*/
//0x00,0x04,0x04,0x04,0x04,0x04,0x04,0xFC,0x04,0x04,0x04,0x04,0x04,0x04,0x00,0x00,
//0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x3F,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00
//};
//const u8 lcd_ju[]={
///*--  ����:  ��  --*/
///*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=16x16   --*/
//0x00,0x00,0x00,0xFE,0x52,0x52,0x52,0x52,0x52,0x52,0x52,0xFE,0x00,0x00,0x00,0x00,
//0x08,0x08,0x88,0x4F,0x29,0x19,0x09,0x09,0x09,0x19,0x29,0x4F,0x88,0x08,0x08,0x00
//};
const u8 lcd_xing[]={
/*--  ����:  ��  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=16x16   --*/
0x20,0x22,0xA2,0x7E,0x22,0x22,0xFE,0x22,0x22,0x00,0x7C,0x00,0x00,0xFF,0x00,0x00,
0x44,0x42,0x49,0x48,0x48,0x48,0x49,0x7E,0x48,0x48,0x48,0x49,0x4A,0x41,0x40,0x00
};
const u8 lcd_hao[]={
/*--  ����:  ��  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=16x16   --*/
0x80,0x80,0x80,0xBE,0xA2,0xA2,0xA2,0xA2,0xA2,0xA2,0xA2,0xBE,0x80,0x80,0x80,0x00,
0x00,0x00,0x00,0x06,0x05,0x04,0x04,0x04,0x44,0x84,0x44,0x3C,0x00,0x00,0x00,0x00
};


const u8 lcd_cuo[]={
/*--  ����:  ��  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=16x16   --*/
0x40,0x30,0xEF,0x24,0x64,0x48,0x48,0x7F,0x48,0x48,0x48,0x7F,0x48,0x48,0x40,0x00,
0x01,0x01,0x7F,0x21,0x11,0x00,0xFF,0x49,0x49,0x49,0x49,0x49,0xFF,0x00,0x00,0x00
};
const u8 lcd_wu[]={
/*--  ����:  ��  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=16x16   --*/
0x40,0x42,0xCC,0x00,0x00,0x80,0x9E,0x92,0x92,0x92,0x92,0x92,0x9E,0x80,0x00,0x00,
0x00,0x00,0x7F,0x20,0x94,0x84,0x44,0x24,0x14,0x0F,0x14,0x24,0x44,0x84,0x84,0x00
};
const u8 lcd_dai[]={
/*--  ����:  ��  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=16x16   --*/
0x00,0x80,0x60,0xF8,0x07,0x20,0x20,0x20,0x7F,0xA0,0x10,0x11,0x16,0x10,0x10,0x00,
0x01,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x03,0x0C,0x10,0x20,0x40,0xF8,0x00
};
const u8 lcd_ma[]={
/*--  ����:  ��  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=16x16   --*/
0x04,0x84,0xE4,0x5C,0x44,0xC4,0x00,0x02,0xF2,0x82,0x82,0x82,0xFE,0x80,0x80,0x00,
0x02,0x01,0x7F,0x10,0x10,0x3F,0x00,0x08,0x08,0x08,0x08,0x48,0x88,0x40,0x3F,0x00
};



const u8 lcd_a[]={
/*--  ����:  a  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3F,0x20
};
const u8 lcd_b[]={
/*--  ����:  b  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x08,0xF8,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x3F,0x11,0x20,0x20,0x11,0x0E,0x00
};
const u8 lcd_c[]={
/*--  ����:  c  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x0E,0x11,0x20,0x20,0x20,0x11,0x00
};
const u8 lcd_d[]={
/*--  ����:  d  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x00,0x00,0x80,0x80,0x88,0xF8,0x00,0x00,0x0E,0x11,0x20,0x20,0x10,0x3F,0x20
};
const u8 lcd_e[]={
/*--  ����:  e  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x22,0x22,0x22,0x22,0x13,0x00
};
const u8 lcd_f[]={
/*--  ����:  f  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x80,0x80,0xF0,0x88,0x88,0x88,0x18,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00
};
const u8 lcd_g[]={
/*--  ����:  g  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x6B,0x94,0x94,0x94,0x93,0x60,0x00
};

const u8 lcd_h1[]={
/*--  ����:  h  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x08,0xF8,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20
};
const u8 lcd_i[]={
/*--  ����:  i  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x80,0x98,0x98,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00
};
const u8 lcd_j[]={
/*--  ����:  j  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x00,0x00,0x80,0x98,0x98,0x00,0x00,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00
};
const u8 lcd_k[]={
/*--  ����:  k  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x08,0xF8,0x00,0x00,0x80,0x80,0x80,0x00,0x20,0x3F,0x24,0x02,0x2D,0x30,0x20,0x00
};
const u8 lcd_l[]={
/*--  ����:  l  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x08,0x08,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00
};
const u8 lcd_m[]={
/*--  ����:  mm  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x20,0x3F,0x20,0x00,0x3F,0x20,0x00,0x3F
};
const u8 lcd_n[]={
/*--  ����:  n  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20
};
const u8 lcd_o[]={
/*--  ����:  o  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00
};
const u8 lcd_p[]={
/*--  ����:  p  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x80,0x80,0x00,0x80,0x80,0x00,0x00,0x00,0x80,0xFF,0xA1,0x20,0x20,0x11,0x0E,0x00
};
const u8 lcd_q[]={
/*--  ����:  q  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x0E,0x11,0x20,0x20,0xA0,0xFF,0x80
};
const u8 lcd_r[]={
/*--  ����:  r  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x20,0x20,0x3F,0x21,0x20,0x00,0x01,0x00
};
const u8 lcd_s[]={
/*--  ����:  s  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x33,0x24,0x24,0x24,0x24,0x19,0x00
};
const u8 lcd_t[]={
/*--  ����:  t  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x80,0x80,0xE0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x1F,0x20,0x20,0x00,0x00
};
const u8 lcd_u[]={
/*--  ����:  u  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x80,0x80,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x1F,0x20,0x20,0x20,0x10,0x3F,0x20
};
const u8 lcd_v[]={
/*--  ����:  v  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x00,0x01,0x0E,0x30,0x08,0x06,0x01,0x00
};
const u8 lcd_w[]={
/*--  ����:  w  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x80,0x80,0x00,0x80,0x00,0x80,0x80,0x80,0x0F,0x30,0x0C,0x03,0x0C,0x30,0x0F,0x00
};
const u8 lcd_x[]={
/*--  ����:  x  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x31,0x2E,0x0E,0x31,0x20,0x00
};
const u8 lcd_y[]={
/*--  ����:  y  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x80,0x81,0x8E,0x70,0x18,0x06,0x01,0x00
};
const u8 lcd_z[]={
/*--  ����:  z  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x21,0x30,0x2C,0x22,0x21,0x30,0x00
};

const u8 lcd_maohao[]={
/*--  ����:  :  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00
};

const u8 lcd_henggan[]={
/*--  ����:  -  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01
};
const u8 lcd_A[]={
/*--  ����:  A  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x00,0xC0,0x38,0xE0,0x00,0x00,0x00,0x20,0x3C,0x23,0x02,0x02,0x27,0x38,0x20
};
const u8 lcd_B[]={
/*--  ����:  B  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x08,0xF8,0x88,0x88,0x88,0x70,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x11,0x0E,0x00
};
const u8 lcd_C[]={
/*--  ����:  C  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0xC0,0x30,0x08,0x08,0x08,0x08,0x38,0x00,0x07,0x18,0x20,0x20,0x20,0x10,0x08,0x00
};
const u8 lcd_D[]={
/*--  ����:  D  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x08,0xF8,0x08,0x08,0x08,0x10,0xE0,0x00,0x20,0x3F,0x20,0x20,0x20,0x10,0x0F,0x00
};
const u8 lcd_E[]={
/*--  ����:  E  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x20,0x23,0x20,0x18,0x00
};
const u8 lcd_F[]={
/*--  ����:  F  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x00,0x03,0x00,0x00,0x00
};
const u8 lcd_G[]={
/*--  ����:  G  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0xC0,0x30,0x08,0x08,0x08,0x38,0x00,0x00,0x07,0x18,0x20,0x20,0x22,0x1E,0x02,0x00
};
const u8 lcd_H1[]={
/*--  ����:  H  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x20,0x3F,0x21,0x01,0x01,0x21,0x3F,0x20
};
const u8 lcd_I[]={
/*--  ����:  I  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00
};
const u8 lcd_J[]={
/*--  ����:  J  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,0x00
};
const u8 lcd_K[]={
/*--  ����:  K  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x08,0xF8,0x88,0xC0,0x28,0x18,0x08,0x00,0x20,0x3F,0x20,0x01,0x26,0x38,0x20,0x00
};
const u8 lcd_L[]={
/*--  ����:  L  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x08,0xF8,0x08,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x20,0x30,0x00
};
const u8 lcd_M[]={
/*--  ����:  M  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x08,0xF8,0xF8,0x00,0xF8,0xF8,0x08,0x00,0x20,0x3F,0x00,0x3F,0x00,0x3F,0x20,0x00
};
const u8 lcd_N[]={
/*--  ����:  N  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x08,0xF8,0x30,0xC0,0x00,0x08,0xF8,0x08,0x20,0x3F,0x20,0x00,0x07,0x18,0x3F,0x00
};
const u8 lcd_O[]={
/*--  ����:  O  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x10,0x20,0x20,0x20,0x10,0x0F,0x00
};
const u8 lcd_P[]={
/*--  ����:  P  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x08,0xF8,0x08,0x08,0x08,0x08,0xF0,0x00,0x20,0x3F,0x21,0x01,0x01,0x01,0x00,0x00
};
const u8 lcd_Q[]={
/*--  ����:  Q  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x18,0x24,0x24,0x38,0x50,0x4F,0x00
};
const u8 lcd_R[]={
/*--  ����:  R  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x08,0xF8,0x88,0x88,0x88,0x88,0x70,0x00,0x20,0x3F,0x20,0x00,0x03,0x0C,0x30,0x20
};
const u8 lcd_S[]={
/*--  ����:  S  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x70,0x88,0x08,0x08,0x08,0x38,0x00,0x00,0x38,0x20,0x21,0x21,0x22,0x1C,0x00
};
const u8 lcd_T[]={
/*--  ����:  T  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x18,0x08,0x08,0xF8,0x08,0x08,0x18,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00
};
const u8 lcd_U[]={
/*--  ����:  U  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00
};
const u8 lcd_V[]={
/*--  ����:  V  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x08,0x78,0x88,0x00,0x00,0xC8,0x38,0x08,0x00,0x00,0x07,0x38,0x0E,0x01,0x00,0x00
};
const u8 lcd_W[]={
/*--  ����:  W  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0xF8,0x08,0x00,0xF8,0x00,0x08,0xF8,0x00,0x03,0x3C,0x07,0x00,0x07,0x3C,0x03,0x00
};
const u8 lcd_X[]={
/*--  ����:  X  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x08,0x18,0x68,0x80,0x80,0x68,0x18,0x08,0x20,0x30,0x2C,0x03,0x03,0x2C,0x30,0x20
};
const u8 lcd_Y[]={
/*--  ����:  Y  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x08,0x38,0xC8,0x00,0xC8,0x38,0x08,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00
};
const u8 lcd_Z[]={
/*--  ����:  Z  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x10,0x08,0x08,0x08,0xC8,0x38,0x08,0x00,0x20,0x38,0x26,0x21,0x20,0x20,0x18,0x00
};


const u8 lcd_0[]={
/*--  ����:  0  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x0F,0x10,0x20,0x20,0x10,0x0F,0x00
};
const u8 lcd_1[]={
/*--  ����:  1  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00
};
const u8 lcd_2[]={
/*--  ����:  2  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x70,0x08,0x08,0x08,0x88,0x70,0x00,0x00,0x30,0x28,0x24,0x22,0x21,0x30,0x00
};
const u8 lcd_3[]={
/*--  ����:  3  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x30,0x08,0x88,0x88,0x48,0x30,0x00,0x00,0x18,0x20,0x20,0x20,0x11,0x0E,0x00
};
const u8 lcd_4[]={
/*--  ����:  4  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x00,0xC0,0x20,0x10,0xF8,0x00,0x00,0x00,0x07,0x04,0x24,0x24,0x3F,0x24,0x00
};
const u8 lcd_5[]={
/*--  ����:  5  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0xF8,0x08,0x88,0x88,0x08,0x08,0x00,0x00,0x19,0x21,0x20,0x20,0x11,0x0E,0x00
};
const u8 lcd_6[]={
/*--  ����:  6  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0xE0,0x10,0x88,0x88,0x18,0x00,0x00,0x00,0x0F,0x11,0x20,0x20,0x11,0x0E,0x00
};
const u8 lcd_7[]={
/*--  ����:  7  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x38,0x08,0x08,0xC8,0x38,0x08,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00
};
const u8 lcd_8[]={
/*--  ����:  8  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x70,0x88,0x08,0x08,0x88,0x70,0x00,0x00,0x1C,0x22,0x21,0x21,0x22,0x1C,0x00
};
const u8 lcd_9[]={
/*--  ����:  9  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x00,0x31,0x22,0x22,0x11,0x0F,0x00
};


const u8 lcd_point[]={
/*--  ����:  .  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00
};

const u8 lcd_blank[]={
	/*--  ����:     --*/
	/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=8x16   --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};