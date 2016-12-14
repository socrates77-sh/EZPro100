
#ifndef _M20P38IRC_H
#define _M20P38IRC_H



extern u16 const M238_Addr0[9];
extern u8  const M238_Data0[9];
extern u16 const M238_Addr1[4];
extern u8 const M238_Data1[4];

extern u8 const M106_Data0[27];
extern u8 const M106_Data1[4];
extern u16 const M106_Addr0[27];
extern u16 const M106_Addr1[4];


extern  u8 M238IRC(u16 OTPID,u8 IRC_Option,u16 IRC_Addr,u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType);
extern  u8 M238OPRATION(u16 ADDR,u8 SendData);
extern  u8 M238FTSendData(u8 IRC_Option, u16 AAdjust,u8 DAdjust);
extern  u8 M238FTest(u8 IRC_Option, u16 IRC_Addr,u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType);


#endif