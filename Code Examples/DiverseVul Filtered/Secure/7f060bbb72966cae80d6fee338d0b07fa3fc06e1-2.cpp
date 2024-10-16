static void long2str(unsigned char *dst, s32 n)
{
	dst[0] = (n    )&0xff;
	dst[1] = (n>> 8)&0xff;
	dst[2] = (n>>16)&0xff;
	dst[3] = (n>>24)&0xff;
}