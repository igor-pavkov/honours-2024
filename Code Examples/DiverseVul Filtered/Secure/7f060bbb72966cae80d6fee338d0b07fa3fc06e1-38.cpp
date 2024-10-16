static u32 str2ulong_key (unsigned char *str)
{
	u32 c = str2ulong(str);
	c &= 0x80000000;
	if (c == 0) return 0x10;
	else return 0;
}