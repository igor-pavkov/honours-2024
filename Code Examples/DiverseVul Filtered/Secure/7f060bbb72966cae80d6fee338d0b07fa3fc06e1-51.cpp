static u64 str2ullong(unsigned char *str)
{
	u64 r = ((u32)str[0] | ((u32)str[1]<<8) | ((u32)str[2]<<16) | ((u32)str[3]<<24));
	u64 s = ((u32)str[4] | ((u32)str[5]<<8) | ((u32)str[6]<<16) | ((u32)str[7]<<24));
#ifdef __GNUC__
	return ((s<<32)&0xffffffff00000000ULL)|(r&0xffffffff);
#else
	return ((s<<32)&0xffffffff00000000)|(r&0xffffffff);
#endif
}