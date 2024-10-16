static void short2str(unsigned char *dst, s32 n)
{
	dst[0] = (n    )&0xff;
	dst[1] = (n>> 8)&0xff;
}