#ifndef MC30P6060_H
#define MC30P6060_H



extern u8 MC30P6060_Program();
extern u8 MC30P6060IRC(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType);
extern void MC30P6060_MODEL_IN(u8 VddFlag);
extern void MC30P6060FTtest_11Bit(u16 irc_adj_value);
extern void MC30P6060FTtest(u8 IRC_Option);
extern u16 MC30P6060_PreRead(u8 command);
extern u16 MC30P6060_R();
extern void MC30P6060_W(u16 OTPData);
extern void MC30P6060_WriteByte(u16 OTPAddr,u16 OTPData);
extern u16 MC30P6060_ReadByte(u16 OTPAddr);
extern void MC30P6060_IRC_INST(u16 OTPData);
extern void MC30P6060_Instruct(u16 OTPData);
#endif	