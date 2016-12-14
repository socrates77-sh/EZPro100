#ifndef MC32P7311_H
#define MC32P7311_H


extern void MC32P7311FTtest_10Bit(u16 irc_adj_value);
extern u8 MC32P7311IRC_10Bit(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType);
extern u8 MC32P7311IRC_10Bit_Check(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType);
extern u8 MC32P7311IRC_10Bit_ModeInCheck(u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType);
//extern u8 MC32P7311_Program();

#endif	