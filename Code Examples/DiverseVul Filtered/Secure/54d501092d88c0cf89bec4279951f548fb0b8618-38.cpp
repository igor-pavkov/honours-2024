static u16 zipfileGetU16(const u8 *aBuf){
  return (aBuf[1] << 8) + aBuf[0];
}