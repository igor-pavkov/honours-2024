int conv_uni_to_8bit(u32 uni)
{
	int c;
	for (c = 0; c < 0x100; c++)
		if (translations[USER_MAP][c] == uni ||
		   (translations[USER_MAP][c] == (c | 0xf000) && uni == c))
			return c;
	return -1;
}