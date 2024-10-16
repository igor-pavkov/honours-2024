static inline unsigned long busy_clock(void)
{
	return local_clock() >> 10;
}