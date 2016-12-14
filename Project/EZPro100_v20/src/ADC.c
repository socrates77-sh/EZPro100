
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
#include "Config.h"
#include "ADC.h"


void ADC_Config(void)
{

	ADC_InitTypeDef  ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_ADCCLKConfig(RCC_PCLK2_Div2);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA,ENABLE);


	GPIO_InitStructure.GPIO_Pin  =GPIO_Pin_1 |GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AIN;
	GPIO_Init(GPIOA,&GPIO_InitStructure); //Ĭ���ٶ�Ϊ����


	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(GPIOA,&GPIO_InitStructure);


	ADC_InitStructure.ADC_Mode              = ADC_Mode_Independent;  //����ģʽ
	ADC_InitStructure.ADC_ScanConvMode      =DISABLE; //ENABLE;      //������ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode =ENABLE;      //����ת��
	ADC_InitStructure.ADC_ExternalTrigConv  = ADC_ExternalTrigConv_None; //ת������������
	ADC_InitStructure.ADC_DataAlign         =ADC_DataAlign_Right;   //�Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel      =1;   //3    //ɨ��ͨ����

	ADC_Init(ADC1, &ADC_InitStructure);

	//ת������1��2��3
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1, 1,ADC_SampleTime_1Cycles5); //ͨ��X,����ʱ��Ϊ1.5����,1�������ͨ����1�����1��ɶ��˼�Ҳ�̫���ֻ����1��ʱ���ҵ�ADC��������
    //    ADC_RegularChannelConfig(ADC1,ADC_Channel_2,2,ADC_SampleTime_1Cycles5);
	//ADC_RegularChannelConfig(ADC1,ADC_Channel_3, 3, ADC_SampleTime_1Cycles5);

	//ADC_DMACmd(ADC1,ENABLE);

	ADC_Cmd  (ADC1,ENABLE);             //ʹ�ܻ���ʧ��ָ����ADC
	//ADC �Զ�У׼����������Ҫִ��һ�Σ���֤����
	// Enable ADC1 reset calibaration register
	ADC_ResetCalibration(ADC1);
	// Check the end of ADC1 reset calibration register
	while(ADC_GetResetCalibrationStatus(ADC1));
	// Start ADC1 calibaration
	ADC_StartCalibration(ADC1);
	// Check the end of ADC1 calibration
	while(ADC_GetCalibrationStatus(ADC1));
	// ADC�Զ�У׼����
    
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//ʹ�ܻ���ʧ��ָ����ADC�����ת����������
}

//void DMA_Config(void)
//{
//	DMA_InitTypeDef DMA_InitStructure;
//
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//ʹ��DMAʱ��
//
//	DMA_DeInit(DMA1_Channel1);
//	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;//adc1�����ݼĴ����豸��ַ�������ֲ�����
//	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&adc_value;//һ����2��Ԫ��������׵�ַ
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
//	//BufferSize=2����ΪADCת��������2��ͨ��
//	//������ã�ʹ����1�������adc_value[0]������2�������adc_value[1]
//	DMA_InitStructure.DMA_BufferSize = 3;
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//	//ѭ��ģʽ������Bufferд�����Զ��ص���ʼ��ַ��ʼ����
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
//	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
//	//������ɺ�����DMAͨ��
//	DMA_Cmd(DMA1_Channel1, ENABLE);
//}



u8 TestAdc(void)
{

	u16 adc;
	//POWER_ON(vpp12,vdd30);
    // MCP42050_ADJ(ADJ_VPP,150);//vpp value
     //MCP42050_ADJ(ADJ_VDD,51); //vdd value
	if (DeviceConfig_xx.MCU_Type==0x01)
	{
		
		ADC_RegularChannelConfig(ADC1,ADC_Channel_1, 1,ADC_SampleTime_1Cycles5);
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)==RESET); //����ƶ�ADC��־λ��1��� ADC_FLAG_EOC ת��������־λ
		adc=ADC_GetConversionValue(ADC1);
		if ((adc>1522) || (adc<1346)) //7.0V~6.2V  ADJ=6.74V
		{
		   ERORR_VALUE=VDD_Test_false;
		   return 0;
		}
		ADC_RegularChannelConfig(ADC1,ADC_Channel_2, 1,ADC_SampleTime_1Cycles5);
		Delay_1ms(1);
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)==RESET); //����ƶ�ADC��־λ��1��� ADC_FLAG_EOC ת��������־λ
		adc=ADC_GetConversionValue(ADC1);

		if ((adc >2775)||(adc<2450))  //13v~11.5v  ADJ=12.27V
		{
			ERORR_VALUE=VPP_Test_false;
			return 0;
		}
	}
	return 1;

}
