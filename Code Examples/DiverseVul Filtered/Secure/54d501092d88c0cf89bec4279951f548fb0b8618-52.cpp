static u32 zipfileGetU32(const u8 *aBuf){
  return ((u32)(aBuf[3]) << 24)
       + ((u32)(aBuf[2]) << 16)
       + ((u32)(aBuf[1]) <<  8)
       + ((u32)(aBuf[0]) <<  0);
}