
#include "stm32f10x.h"
#include "wx_i2c.h"
#include "global.h"
#include "erorrNum.h"


#define IIC_Write_Address 0xa0 //Write Address
#define IIC_Read_Address 0xa1 //Read Address


#define IIC_SDA_DAT   GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) 

#define IIC_SDA_L   GPIO_ResetBits(GPIOB, GPIO_Pin_5)
#define IIC_SDA_H   GPIO_SetBits(GPIOB, GPIO_Pin_5)
#define IIC_SCL_L   GPIO_ResetBits(GPIOB, GPIO_Pin_6)
#define IIC_SCL_H   GPIO_SetBits(GPIOB, GPIO_Pin_6)

//S/N serial number
// on/off,length,AddrH,AddrL,IDstrat_3,2,1,0,IDend3,2,1,0,index
//    1/0,  1-4 , 0x18, 0x00,  0	  ,0,0,1,  1   ,0,0,0,  1 ; on 4,0x1800, 0001



u8 dynamic_id_add()
{
	u32 id_a_current=0;
	u32 id_a_end=0;
	u8 index;
        u8 rew;

	index=iic_data[14];
	//id_length=iic_data[1];
	/*id_addr0=(iic_data[2]<<8)+iic_data[3];
	id_addr1=(iic_data[4]<<8)+iic_data[5];
	id_addr2=(iic_data[6]<<8)+iic_data[7];
	id_addr3=(iic_data[8]<<8)+iic_data[9];*/

		if (iic_data[0]==0x0a)
		{
			id_a_current=(iic_data[10]<<24)+(iic_data[11]<<16)+(iic_data[12]<<8)+iic_data[13];
			id_a_end=(iic_data[15]<<24)+(iic_data[16]<<16)+(iic_data[17]<<8)+iic_data[18];

			id_a_current+=index;

			if (iic_data[1]==0x01)
			{
				id_a_current &=0xff;
			}
			else if (iic_data[1]==0x02)
			{
				id_a_current &=0xffff;
			}
			else if (iic_data[1]==0x03)
			{
				id_a_current &=0xffffff;
			}
			else
			{
				id_a_current &=0xffffffff;
			}
                        
                        rew=0;
			while(rew<4)
			{
				IIC_Write(13,(id_a_current& 0xff));
				IIC_Write(12,((id_a_current>>8)&0xff));
				IIC_Write(11,((id_a_current>>16)&0xff));
				IIC_Write(10,((id_a_current>>24)&0xff));

				//OKcounter=OKcounter+1;
				IIC_Write(22,(OKcounter& 0xff));
				IIC_Write(21,((OKcounter>>8)&0xff));
				IIC_Write(20,((OKcounter>>16)&0xff));
				IIC_Write(19,((OKcounter>>24)&0xff));
				
				index=IIC_Read(22);

				if (index ==(OKcounter & 0xff))
				{				
					break;
				}
				else if (rew==3)
				{
					ERORR_VALUE=EEPROM_Write_ERROR;
					return 0;
				}				
			}
			if (id_a_end<OKcounter)
			{
				ERORR_VALUE=OK_Counter_Full;
				return 0;
			}

		}

		return 1;
}

void delay_5us(u16 num)//满足IIC时序的延时
{
	u16 i,j;
	for(i=num;i>0;i--)
	{
		for(j=5;j>0;j--);
	}
}

void IIC_Init(void)//IIC初始化程序
{
IIC_SDA_H;
IIC_SCL_H;
delay_5us(1);
}

void IIC_Start(void)//IIC开始信号
{
IIC_SDA_H;
IIC_SCL_H;
delay_5us(1);
IIC_SDA_L;
delay_5us(1);
IIC_SCL_L;
}

void IIC_Stop(void)//IIC停止信号
{
IIC_SDA_L;
IIC_SCL_H;
delay_5us(1);
IIC_SDA_H;
}

void IIC_Wait_ack(void)//IIC主器件等待应答  ?????
{
     u16 ErrTime=500;
    IIC_SCL_H;
//while(IIC_SDA&&(ErrTime>0))
	while(IIC_SDA_DAT)
	{
		ErrTime--;
                if(ErrTime<2)
                  break;
	}
delay_5us(1);
IIC_SCL_L;
}


void IIC_Send_ack(void)//IIC主器件发送应答
{
IIC_SDA_L;
delay_5us(1);
IIC_SCL_H;
delay_5us(1);
IIC_SCL_L;
}


void IIC_Send_noack(void)//IIC主器件发送非应答
{
IIC_SDA_H;
IIC_SCL_H;
delay_5us(1);
IIC_SCL_L;
}


void IIC_Writebyte(u8 Data)//IIC写一个字节
{
u8 i;
for(i=8;i>0;i--)
{
   IIC_SCL_L;
   if((Data&0x80)==0x80)
   {
    IIC_SDA_H;
   }
   else
   {
    IIC_SDA_L;
   }
   Data<<=1;
   delay_5us(1);
   IIC_SCL_H;
   delay_5us(1);
}
IIC_SCL_L;
IIC_SDA_H;
delay_5us(10);
}


u8 IIC_Readbyte(void)//IIC读一个字节
{
u8 i,Data=0;
IIC_SDA_H;
for(i=8;i>0;i--)
{
   Data<<=1;
   IIC_SCL_L;
   delay_5us(1);
   IIC_SCL_H;
   delay_5us(1);
//   Data|=GPIO_ReadInputDataBit(GPIOB, GPIO_Pin7) 
   Data|=IIC_SDA_DAT; //读管脚的状态
}
IIC_SCL_L;
delay_5us(1);
return(Data);
}


void IIC_Write(u16 Address,u8 Data)//IIC 往指定地址Address里写Data
{
u8 addr;
addr = Address&0xff;
//page = (Address>>8)&0xf;
IIC_Start();
IIC_Writebyte(IIC_Write_Address);
IIC_Wait_ack();
//IIC_Writebyte(page);
//IIC_Wait_ack();
IIC_Writebyte(addr);
IIC_Wait_ack();
IIC_Writebyte(Data);
IIC_Wait_ack();
IIC_Stop();
delay_5us(10000);
}


void IIC_Write_Array(u8 *Data,u16 Address,u16 Num)//IIC 往起始地址Address里写Data[]数组
{
u8 i;
u8 *p;
u8 page1,page2,yushu;
u8 page,addr,addr1;
addr = Address&0xff;
page1 = Address>>5;
page2 =(Address+Num)>>5;
addr1 = (Address+Num)&0xff;
p=Data;
for(page=page1;page<=page2;page++)
{
if((page==page1)||(page==page2))
{
   if(page==page1)
   	{
   	 if(page1==page2)
   	 	{
                yushu= Num;
   	           }
   	
    else
    	{
		yushu=(32-addr&0x1f)%32;
    	}
   	}
   else 
   	yushu=addr1%32;
}
else
{
	yushu=0;
}
IIC_Start();
IIC_Writebyte(IIC_Write_Address);
IIC_Wait_ack();
IIC_Writebyte(page>>3);
IIC_Wait_ack();
IIC_Writebyte(addr);
IIC_Wait_ack();
for(i=0;i<32;i++)
{
addr++;
IIC_Writebyte(*p);
IIC_Wait_ack();
p++;
if(yushu!=0)
{
	if(i==yushu-1)
	break;
}
}
IIC_Stop();
delay_5us(10000);
}  
}


u8 IIC_Read(u16 Address)//IIC 读指定地址Address里的Data
{
u8 Data;
u8 addr;
addr = Address&0xff;
//page = (Address>>8)&0xf;
IIC_Start();
IIC_Writebyte(IIC_Write_Address);
IIC_Wait_ack();
//IIC_Writebyte(page);
//IIC_Wait_ack();
IIC_Writebyte(addr);
IIC_Wait_ack();
IIC_Start();
IIC_Writebyte(IIC_Read_Address);
IIC_Wait_ack();
Data=IIC_Readbyte();
IIC_Send_noack();
IIC_Stop();
return(Data);  
}


void IIC_Read_Array(u8 *Data,u16 Address,u16 Num)//IIC 读起始地址Address里的Data[]
{
u16 i;
u8 *p;
u8 page,addr;
addr = Address&0xff;
page = Address>>8;
p=Data;   
IIC_Start();
IIC_Writebyte(IIC_Write_Address);
IIC_Wait_ack();
IIC_Writebyte(page);
IIC_Wait_ack();
IIC_Writebyte(addr);
IIC_Wait_ack();
IIC_Start();
IIC_Writebyte(IIC_Read_Address);
IIC_Wait_ack();
for(i=0;i<Num;i++)
{
*(p+i)=IIC_Readbyte();
if(i==Num-1)
	IIC_Send_noack();
else
	IIC_Send_ack();
}
IIC_Stop();
}


void IIC_Clear(u16 Address,u16 Num)
{
u8 i,j;
u8 page1,page2,yushu;
u8 page,addr,n,addr1;
addr = Address&0xff;
page1 = (Address>>8)&0xf;
page2 =((Address+Num)>>8)&0xf;
addr1 = (Address+Num)&0xff;
for(page=page1;page<=page2;page++)
{
if((page==page1)||(page==page2))
{
   if(page==page1)
   	{
   	 if(page1==page2)
   	 	{
	 	n=Num/32;
	 yushu=Num%32;
   	 	}
	  else
	  	{
   	n=(256-addr)/32;
	 yushu=(256-addr)%32;
	  	}
	 if(yushu)
	 	n+=1;
   	}
    else
    	{
    	n=addr1/32;
		yushu=addr1%32;
		if(yushu)
			n+=1;
    	}
}
else
{
	n=8;
	yushu=0;
}
for(j=0;j<n;j++)
{
IIC_Start();
IIC_Writebyte(IIC_Write_Address);
IIC_Wait_ack();
IIC_Writebyte(page);
IIC_Wait_ack();
IIC_Writebyte(addr+32*j);
IIC_Wait_ack();
for(i=0;i<32;i++)
{
IIC_Writebyte(0xff);
IIC_Wait_ack();
}
IIC_Stop();
delay_5us(10000);
}
}
}

/*******************************************************************************************************
                                 end  file!!!
********************************************************************************************************/

