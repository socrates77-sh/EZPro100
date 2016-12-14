#ifndef MC32P7011_H
#define MC32P7011_H


//power value
#define vpp65   1       //6.5V
#define vpp00   0

#define vdd50   1       //5.0V
#define vdd00   0

#define VPP_On  GPIO_SetBits(GPIOA,GPIO_Pin_9)
#define VPP_Off GPIO_ResetBits(GPIOA,GPIO_Pin_9)

#define VDD_On  GPIO_SetBits(GPIOA,GPIO_Pin_0)
#define VDD_Off GPIO_ResetBits(GPIOA,GPIO_Pin_0)


extern u8 MC32P7011_Program();
extern void MC32P7011_MODEL_IN(u8 VddFlag);

#endif	