#ifndef MC32P7022_H
#define MC32P7022_H


//power value
#define vpp65   1       //6.5V
#define vpp00   0

#define vdd50   1       //5.0V
#define vdd00   0

#define VPP_On  GPIO_SetBits(GPIOA,GPIO_Pin_9)
#define VPP_Off GPIO_ResetBits(GPIOA,GPIO_Pin_9)

#define VDD_On  GPIO_SetBits(GPIOA,GPIO_Pin_0)
#define VDD_Off GPIO_ResetBits(GPIOA,GPIO_Pin_0)


//extern u8 MC32P7022_Program();
//extern u8 MC32T8132IRC(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType);
//extern u8 MC32T8132IRC_8Bit(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType);

extern void MC32P7022FTtest_11Bit(u16 irc_adj_value,u8 TempADJ);
extern u8 MC32P7022IRC_11Bit(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType);
extern u8 MC32P7022IRC_11Bit_Check(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType);
extern u8 MC32P7022IRC_11Bit_ModeInCheck(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType);

#endif	