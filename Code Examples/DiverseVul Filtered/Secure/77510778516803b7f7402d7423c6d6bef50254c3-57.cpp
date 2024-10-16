u32 get_size_by_code(u32 code)
{
	if (code==0) return 4;
	if (code==1) return 8;
	if (code==2) return 16;
	return 32;
}