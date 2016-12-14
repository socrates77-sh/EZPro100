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
#include "power.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "USB_Rx_Tx.h"
#include "IAP.h"
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
 
  //----------------------------------
   //u16 OKCnt=0;
	 FLASH_Unlock();
  /* System Clocks Configuration */
  RCC_Configuration();

  /* Configure the GPIO ports */
  GPIO_Configuration();
  LEDOK_Off;
  LEDNG_Off;
  LEDBUSY_Off; 
  
  USB_REG_Off; //
  PowerInitial();
  POWER_OFF(vpp00,vdd00);
   
	//Set_System();
	//Set_USBClock();
	//USB_Interrupts_Config();
	//USB_Init();
	////USB_REG_On;
//-------------------------------------  
  StateFlag =GPIO_ReadInputDataBit(KEY);     


  	if(StateFlag==0) //iap is enable
	{	
            GPIO_SetBits(GPIOA,GPIO_Pin_10);//beeper enable
	        Delay_1ms(200);
	        GPIO_ResetBits(GPIOA,GPIO_Pin_10);
			  
            Set_System();
            Set_USBClock();
            USB_Init();
            USB_REG_On;
            USB_Interrupts_Config();
              
            
              USB_Rx_Flag=0x00;
              USB_RxCommand=0x00;
              
	      IAP_funtion(); 
	}
	else //user code
	{
		//�ж��û��Ƿ��Ѿ����س�����Ϊ��������´˵�ַ��ջ��ַ��
		//��û����һ��Ļ�����ʹû�����س���Ҳ�����������ܷɡ�
		if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
		{
			//SerialPutString("Execute user Program\r\n\n");
			//��ת���û�����
			JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
			Jump_To_Application = (pFunction) JumpAddress;

			//��ʼ���û�����Ķ�ջָ��
			__set_MSP(*(__IO uint32_t*) ApplicationAddress);
			Jump_To_Application();
                }
	}
 
  while (1)
  {
	
  }//whil(1)
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
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
