static inline int inword(const u16 c) {
	return c > 0xff || (( inwordLut[c>>5] >> (c & 0x1F) ) & 1);
}