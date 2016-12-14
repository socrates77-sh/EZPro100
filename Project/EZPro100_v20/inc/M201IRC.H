
#ifndef _M201IRC_H
#define _M201IRC_H

#define ID_M201_TEST  0x0118
#define IRC_M201_OPTION 0x3d
#define IRC_Addr_M201  0x1c01
#define M201_FrMax   127
#define M201_FrMin   123
#define M201_FrType  125



//extern u16 const M201_Addr0[15];
//extern u8  const M201_Data0[15];
//extern u16 const M201_Addr1[4];
//extern u8 const M201_Data1[4];

#define ID_M224B_TEST  0x0118
#define IRC_M224B_OPTION 0xc7
#define IRC_Addr_M224B 0x1001
#define M224B_FrMax   253 //127
#define M224B_FrMin   247 //123
#define M224B_FrType  250 //125

 
//extern u8 const M224B_Data0[9];
//extern u8 const M224B_Data1[4];
//extern u16 const M224B_Addr0[9];
//extern u16 const M224B_Addr1[4];
//
//
extern  u8 M201IRC(u16 OTPID,u8 IRC_Option,u16 IRC_Addr,u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType);
extern  u8 M201OPRATION(u16 ADDR,u8 SendData);
extern  u8 M201FTSendData(u8 IRC_Option, u16 AAdjust,u8 DAdjust);
extern  u8 M201FTest(u8 IRC_Option, u16 IRC_Addr,u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType);
extern  u8 MC20P02BIRC_ModeInCheck(u16 OTPID,u8 IRC_Option,u16 IRC_Addr,u16 IRC_FreqMin,u16 IRC_FreqMax,u16 IRC_FreqType);


#endif