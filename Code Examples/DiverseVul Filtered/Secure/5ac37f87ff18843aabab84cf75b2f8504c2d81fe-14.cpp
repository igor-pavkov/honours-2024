static inline void clear_LDT(void)
{
	set_ldt(NULL, 0);
}