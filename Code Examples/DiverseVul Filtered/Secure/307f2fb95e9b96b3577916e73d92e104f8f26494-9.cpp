static __inline__ u32 fib6_new_sernum(void)
{
	u32 n = ++rt_sernum;
	if ((__s32)n <= 0)
		rt_sernum = n = 1;
	return n;
}