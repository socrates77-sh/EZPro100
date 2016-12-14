#ifndef MC32P5222Pro_H
#define MC32P5222Pro_H

#define         CRC_16_POLYNOMIALS      0x8005

extern u8 MC32P5222_Program();
extern u8 MC32P5222_MTPByteRead(u16 address);
extern void MC32P5222_MODEL_IN(u8 VddFlag);

#endif	