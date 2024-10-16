static inline unsigned long native_store_tr(void)
{
	unsigned long tr;
	asm volatile("str %0":"=r" (tr));
	return tr;
}