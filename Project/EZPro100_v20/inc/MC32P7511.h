#ifndef MC32P7511_H
#define MC32P7511_H

#define         CRC_16_POLYNOMIALS      0x8005

extern u8 MC32P7511_Program();
extern void MC32P7511_MODEL_IN(u8 VddFlag);
extern u8 MC32P5222_MTPByteRead(u16 address);

#endif	