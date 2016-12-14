#ifndef MC32E22_H
#define MC32E22_H


//power value
#define vpp85   1       //8.5V
#define vpp00   0

#define vdd50   1       //5.0V
#define vdd00   0

#define VPP_On  GPIO_SetBits(GPIOA,GPIO_Pin_9)
#define VPP_Off GPIO_ResetBits(GPIOA,GPIO_Pin_9)

#define VDD_On  GPIO_SetBits(GPIOA,GPIO_Pin_0)
#define VDD_Off GPIO_ResetBits(GPIOA,GPIO_Pin_0)


extern u8 MC32E22_Program();
extern u8 MC32E22IRC(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType);

#endif	