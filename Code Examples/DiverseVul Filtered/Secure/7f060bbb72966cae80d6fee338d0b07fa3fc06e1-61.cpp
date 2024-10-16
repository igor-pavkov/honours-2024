static u32 str2ushort(unsigned char *str)
{
	return ( (u32)str[0] | ((u32)str[1]<<8) );
}