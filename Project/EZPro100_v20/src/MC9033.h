#ifndef MC9033_H
#define MC9033_H

//commands were define in mc30p01pro.h file


extern u8 MC9033_Promgram();
extern u16 MC321_PreRead(u8 command);
extern u16 MC321_R( );
extern void MC321_W(u16 OTPData);
extern u16 MC321_ReadByte(u16 OTPAddr);
extern void MC321_Instruct(u16 OTPDdata);
extern void MC321_WriteByte(u16 OTPAddr,u16 OTPData);
extern u8 M321IRC(u16 IRC_FreMin,u16 IRC_FreqMax,u16 IRC_FreqType);
extern u8 M321IRC_11Bit(u16 IRC_FreMin,u16 IRC_FreqMax,u16 IRC_FreqType);
extern void MC321_IRC_INST(u16 OTPData);
extern void MC321FTtest(u8 irc_adj_value);
extern void MC321FTtest_11Bit(u16 irc_adj_value);
extern void MC321_MODEL_IN(u8 VddFlag);

extern u8 MC3081IRC(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType);

extern u8 M9029IRC(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType);
extern u8 M9033IRC(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType);
extern void M9029FTtest(u16 irc_adj_value,u8 tempadj);
extern void MC9033_MODEL_IN(u8 VddFlag);

extern u8 M31P11IRC_11Bit(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType);
//extern void MC31P11FTtest_11Bit(u16 irc_adj_value);
extern void MC31P11FTtest_11Bit(u16 irc_adj_value,u8 TempADJ);

extern void MC9033FTtest(u16 irc_adj_value,u8 tempadj);
extern void MC9033_IRC_INST(u16 OTPData);
extern void MC9033_SetRmode(u8 ReadMode);
extern u16 MC9033_ReadByte_DiffMode(u8 ModeNum,u16 OTPAddr);
extern u16 MC9033_R_DiffMode(u8 ModeNum);

#endif	