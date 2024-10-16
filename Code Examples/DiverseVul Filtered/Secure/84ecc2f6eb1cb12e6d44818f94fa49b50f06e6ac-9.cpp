u32 conv_8bit_to_uni(unsigned char c)
{
	unsigned short uni = translations[USER_MAP][c];
	return uni == (0xf000 | c) ? c : uni;
}