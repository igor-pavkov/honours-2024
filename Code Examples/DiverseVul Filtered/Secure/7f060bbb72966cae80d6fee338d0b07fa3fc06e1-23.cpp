static u32 str2ulong_len (unsigned char *str)
{
	return str2ulong(str) & 0x7fffffff;
}