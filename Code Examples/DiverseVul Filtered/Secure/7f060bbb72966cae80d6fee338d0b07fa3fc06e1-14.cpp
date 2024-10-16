static u32 str2ulong(unsigned char *str)
{
	return ( (u32)str[0] | ((u32)str[1]<<8) | ((u32)str[2]<<16) | ((u32)str[3]<<24) );
}