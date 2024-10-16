static inline unsigned char div255(int x) {
  return (unsigned char)((x + (x >> 8) + 0x80) >> 8);
}