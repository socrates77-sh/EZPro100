#ifndef erorrNum_h
#define erorrNum_h

//MC20P24B_Program
#define		IRC_ModeIn_false	2	//IRCУ׼ ����ģʽʧ��
#define		IRC_Value_false		3	//IRCУ׼Ƶ��ֵ ������Χ
#define		OTP_ModeIn_false	4	//������дģʽʧ��
#define		VDD_Test_false		5	//VDD��ѹ���� 7.0V~6.2V��Χ
#define		VPP_Test_false		6	//VPP��ѹ����13V~~11.5V��Χ
#define		IRC_Write_false		7	//IRCУ׼ֵ��дʧ��
#define		ROM_Write_false		8	//ROM��������дʧ��

#define		FM_Read_false		  9	//���򻺴�����ȡʧ��
#define		OPTION_Write_false	10	//OPTIONֵ��дʧ��
#define		Program_Erorr		11	//��д�쳣
#define		IRC_Value_0XFF		12	//У׼���Ϊ 0XFF or 0x00
#define		EEPROM_Write_ERROR	13   //дEEPROM ����
#define		OK_Counter_Full		14	 //��д�����ﵽ���������
#define		VS_Low				15	 //�汾��̫��
#define     ADDR_EER            16   //OTP��ַ����д��ַ��һ��

//MC32T8132 ��������ID
#define         ERASE_STA_xx0       17
#define         ERASE_STA_011       18
#define         ERASE_STA_101       19
#define         ERASE_STA_111       20


//5312���Ӵ���
#define         HIRC_DIFF       21//оƬ��д�������ұ����õ��ڲ���Ƶ����Ƶ�ʺ�֮ǰ�Ĳ�һ��

//MC33P116 ʧ�ܲ�����
#define         NOMALL_ERROR           22//��ͨ����
#define         MAGRIN1_ERROR          23//
#define         OFF_MAGRIN_ERROR       24
#define         CHECK0_ERROR           25

//MC32P5222 MTP��д����
#define         MTP_Write_false        26

#define         CHECK_PROGRAM_ERROR     27 //����ռ�У�����    
#define         NO_VREF_ERROR           28//��VREFУ׼ֵ
#define         NO_LVD_ERROR            29//��LVDУ׼ֵ    
#define         OPTION_MCU_ERROR        30 //оƬ�ͺ������ò�ƥ��
#define         OPTION_NOMALL_ERROR           31//OPTION��ͨ����
#define         OPTION_MAGRIN1_ERROR          32//
#define         OPTION_OFF_MAGRIN_ERROR       33
#define         S19_ERROR                     34

#endif