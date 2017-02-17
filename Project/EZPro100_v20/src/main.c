/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : Virtual Com Port Demo main file
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include "stm32f10x.h"

#include "stm32f10x_tim.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "global.h"
#include "erorrNum.h"
#include "spi.h"
#include "power.h"
#include "OTPRW.h"
#include "IRC.h"
#include "M201IRC.h"
#include "MC224BPro.h"
#include "MC20P02BPro.h"
#include "MC301Pro.h"
#include "Config.h"
#include "ADC.h"
#include "OTPRW_8K.h"
#include "MC20P38Pro.h"
#include "MC32P21Pro.h"
#include "MC9033.h"

#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "USB_Rx_Tx.h"
#include "LCD.h"
#include "wx_i2c.h"

//#define SINOMCU_IAP
/*******************************************************************************
* Function Name  : main. Writer only
* Description    : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.c
*******************************************************************************/
int main(void)
{
  //u16 i;
  //-----------------------------------
  u8 OKorNG;
  u8 Retry = 0;
  // u8 ProgramID;
  //u16 OKCnt=0;
  /* System Clocks Configuration */
  RCC_Configuration();
  /* Configure the GPIO ports */
  GPIO_Configuration();

//  NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x4000);
// zwr: 这句可以注释掉吗？
#ifdef SINOMCU_IAP
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);
#endif

  LEDOK_Off;   // zwr: GPIOC.10
  LEDNG_Off;   // zwr: GPIOC.11
  LEDBUSY_Off; // zwr: GPIOC.12

  //USB_REG_Off;
  IRCInitial(); //OSCO ��OSCI�ò���

  ConfigTimer();
  ConfigTIM4();
  /* spi2 configuration  */
  SPI2Init();
  //IIC initial
  IRCInitial(); //zwr: 为什么执行了2次
  /* ADC initail  */
  ADC_Config();
  //DMA_Config();

  PowerInitial(); //zwr: 电源初始化，全部关

  OTPInit(); //zwr: SCK, SDI set to 0
  VDD30V_Off;
  POWER_OFF(vpp00, vdd00); //zwr: 为什么电源又关一遍
  Power18V_On;

  GPIO_SetBits(GPIOA, GPIO_Pin_10); //zwr: 这个引脚翻转一次，且延时300ms？做什么用
  Delay_1ms(300);
  GPIO_ResetBits(GPIOA, GPIO_Pin_10);

  OKorNG = 1;
  OKcounter = 0;
  NGcounter = 0;
  ERORR_VALUE = 0;
  //MCP42050_ADJ(ADJ_VPP,150);//vpp value
  //MCP42050_ADJ(ADJ_VDD,51); //vdd value
  //OKorNG =TestAdc();

  //adj_vdd
  Retry = GPIO_ReadInputData(GPIOD);
  if (Retry == 0xff)
    Writer_Test();

  //else
  //  Retry &=0x07 ;
  //  switch(Retry)
  //{
  // case 0:
  //  DeviceConfig_xx=DeviceConfig_M224;

  //  MCP42050_ADJ(ADJ_VDD,51);
  //  break;
  // case 1:
  //  DeviceConfig_xx=DeviceConfig_M202;
  //  MCP42050_ADJ(ADJ_VDD,51);
  //  break;
  // case 2: //mc20p34
  //  DeviceConfig_xx=DeviceConfig_M238;
  //  MCP42050_ADJ(ADJ_VDD,51);
  //        MCP42050_ADJ(ADJ_VPP,150);//vpp value
  //  break;
  // case 0x03:
  //  DeviceConfig_xx=DeviceConfig_M102R;
  //  MCP42050_ADJ(ADJ_VDD,25);
  //        break;
  // case 0x04:
  //  DeviceConfig_xx=DeviceConfig_M111;
  //  MCP42050_ADJ(ADJ_VDD,25);
  //  break;
  // case 0x05:
  //  DeviceConfig_xx=DeviceConfig_M101;
  //  MCP42050_ADJ(ADJ_VDD,25);
  //  break;
  //       case 0x07:
  //        DeviceConfig_xx=DeviceConfig_T201;
  //  MCP42050_ADJ(ADJ_VDD,51);
  //  break;
  // case  0xff:
  //  Writer_Test();
  //  break;
  // default:
  //  //ProgramID=3;
  //  MCP42050_ADJ(ADJ_VDD,57);
  //}

  DeviceConfig(); //no use this funtion when key select type
  //DeviceConfig_xx.ProVPP=0x46;
  MCP42050_ADJ(ADJ_VPP, (u8)DeviceConfig_xx.ProVPP); //vpp value
  MCP42050_ADJ(ADJ_VDD, (u8)DeviceConfig_xx.IrcVDD);
  //  MCP42050_ADJ(ADJ_VDD,25);
  //  DeviceConfig_xx.ProgramID=0x04;
  //  DeviceConfig_xx.MCU_ID=0x5222;

  USB_RxDataCS = CalculateCheckSum();
  if (DeviceConfig_xx.MCU_ID == 0x3081 || DeviceConfig_xx.MCU_ID == 0x6060)
  {
    USB_RxDataCS &= 0x3fff;
  }
  else if (DeviceConfig_xx.MCU_ID == 0x3401)
  {
    USB_RxDataCS &= 0x1fff;
  }

  //----------LCD display initial --------------------------
  for (Retry = 0; Retry < IIC_DATA_BUFF_SIZE; Retry++) // dynamic id
  {
    iic_data[Retry] = IIC_Read(Retry);
  }
  Initial_Lcd();

  displayfuntion(); //LCD
  //Power18V_Off;
  //-----------------------------------

  Set_System();
  Set_USBClock();
  USB_Init();
  USB_REG_On;
  USB_Interrupts_Config();

  //-------------------------------------
  USB_Rx_Flag = 0x00;
  USB_RxCommand = 0x00;
  StateFlag = 0xfe;

  //  MC32P8132_Program();
  while (1)
  {
    //RCC_MCOConfig(RCC_MCO_NoClock);
    //RCC_MCOConfig(RCC_MCO_PLLCLK_Div2);

    if (USB_Rx_Flag == 0xaa && USB_Rx_Buffer[0] == 0x68)
    {
      USB_Rx_Tx();
      //Retry=0;
    }
    else if (USB_Rx_Flag == 0xaa && USB_Rx_Buffer[0] != 0x68)
    {
      USB_Rx_Flag = 0x00;
      //#ifndef STM32F10X_CL
      /* Enable the receive of data on EP3 */
      SetEPRxValid(ENDP3);
      //#endif /* STM32F10X_CL */
    }
    //----------------------------------
    //---------------- key scan ---------------------------------
    if (GPIO_ReadInputDataBit(KEY) == 0)
    {
      Delay_1ms(5);
      if (GPIO_ReadInputDataBit(KEY) == 0)
      {
        StateFlag = GPIO_ReadInputData(GPIOD) & 0x00fe;
        //StateFlag = WR_Command; //
        LEDNG_Off;
        LEDOK_Off;

        for (Retry = 0; Retry < IIC_DATA_BUFF_SIZE; Retry++) // dynamic id
        {
          iic_data[Retry] = IIC_Read(Retry);
        }
      }
      while (GPIO_ReadInputDataBit(KEY) == 0)
        ;
    }
    //----------------------------------------------
    if (StateFlag != 0xfe)
    {
      LEDBUSY_On;
      LEDNG_Off;
      LEDOK_Off;
      Retry = 0;
      if ((StateFlag & 0x80) == 0) //Read
      {
        //Read command -------------------------
        StateFlag |= 0x08; //IF Read active,don't Write mcu

        //OKorNG=OTP_DownLoad();
        while (++Retry < 10)
        {
          if (OTP_DownLoad(DeviceConfig_xx.MCU_ID, DeviceConfig_xx.RomFirAddr))
          {
            OKorNG = 1;
            break;
          }
          else if (Retry == 8)
          {
            OKorNG = 0;
            break;
          }
        }
        //POWER_OFF(vpp00,vdd00);
      }

      //blank
      if ((StateFlag & 0x20) == 0)
      {
        //Check Blank command
        LEDBUSY_On;
        LEDBUSY_Off;
      }
      //Write ---------------------------------
      if ((StateFlag & 0x08) == 0)
      {
        //OKorNG=OTP_Program();
        while (++Retry < 5)
        {
          if (DeviceConfig_xx.ProgramID == 0x00)
          {
            if (DeviceConfig_xx.MCU_ID == 0x202)
            {
              if (MC20P02B_Program())
              {
                OKorNG = 1;
                break;
              }
            }
            else
            {
              if (MC224B_Program())
              {
                OKorNG = 1;
                break;
              }
            }
          }
          //else if (DeviceConfig_xx.ProgramID==0x01) //M202
          //{
          //	if (OTP_Program()==1)
          //	{
          //		OKorNG=1;
          //		break;
          //	}
          //}
          else if (DeviceConfig_xx.ProgramID == 0X02) //M111
          {
            if (MC301_Program())
            {
              OKorNG = 1;
              break;
            }
          }
          else if (DeviceConfig_xx.ProgramID == 0X03) //M20P38
          {
            if (MC20P38_Program())
            {
              OKorNG = 1;
              break;
            }
          }
          else if (DeviceConfig_xx.ProgramID == 0x04) //MC32P21
          {
            //  if (MC32P21_Program())//��ǰ��
            if (MC32P21_Program_New()) //���ڵ�
            {
              OKorNG = 1;
              break; //��������ע�ͣ���д�ɹ����˳����ڵ�whileѭ��
            }
          }
          else if (DeviceConfig_xx.ProgramID == 0x05) //MC9033
          {
            if (MC9033_Promgram())
            {
              OKorNG = 1;
              break; //��������ע�ͣ���д�ɹ����˳����ڵ�whileѭ��
            }
          }
          //if(Retry ==3)
          if (Retry == 1) //������
          {
            OKorNG = 0;
            break;
          }
          FM_CS_1;
          POWER_OFF(vpp00, vdd00);
          //ConfigTimer();
          ConfigTIM4();
          Delay_1ms(100);
          Delay_1ms(200);
        }
        // ok counter & dynamic id process
        if (OKorNG)
        {
          OKcounter = OKcounter + 1;
          OKorNG = dynamic_id_add();
        }
        else
        {
          NGcounter = NGcounter + 1;
        }
        if (USB_RxCommand == 0x01)
        {
          if (OKorNG == 1)
          {
            for (Retry = 0; Retry < 5; Retry++)
            {
              USB_Tx_Buffer[Retry] = ProgrammingFinished[Retry];
            }
            USB_SendData(5);
          }
          else
          {
            USB_Tx_Buffer[0] = 0x68;
            USB_Tx_Buffer[1] = 0x06;
            USB_Tx_Buffer[2] = 0xf2;
            USB_Tx_Buffer[3] = 0x03;
            USB_Tx_Buffer[4] = OTP_ADDR / 0x100;
            USB_Tx_Buffer[5] = OTP_ADDR % 0x100;
            USB_Tx_Buffer[6] = Rxdata;
            USB_Tx_Buffer[7] = OTP_ReadData;
            USB_Tx_Buffer[8] = 0xfb + USB_Tx_Buffer[4] + USB_Tx_Buffer[5] + USB_Tx_Buffer[6] + USB_Tx_Buffer[7];
            USB_Tx_Buffer[9] = 0x16;
            USB_SendData(10);
          }
        }
        //POWER_OFF(vpp00,vdd00);
      }
      //----------------------------------------
      if ((StateFlag & 0x10) == 0) //Verify
      {
        //POWER_ON(vpp12,vdd65);
        //Delay_1ms(50);
        if (DeviceConfig_xx.ProgramID == 0x03)
        {
          OKorNG = OTP8K_Verify();
        }
        else
        {
          OKorNG = OTP_Verify();
        }
        POWER_OFF(vpp00, vdd00);
      }
      //----------------------------------------
      //if ((StateFlag & 0x40)==0) //Protect
      //{
      //	LEDBUSY_On;
      //	LEDBUSY_Off;
      //}

      FM_CS_1;
      POWER_OFF(vpp00, vdd00);
      ConfigTIM4();
      LEDBUSY_Off;

      Freq(OKorNG);

      StateFlag = 0xfe;
      USB_RxCommand = 0x00;
      //Delay_1ms(200);
      //Delay_1ms(200);
      //while(GPIO_ReadInputDataBit(KEY)==0); //waite key off
    }
  } //whil(1)
}
#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
