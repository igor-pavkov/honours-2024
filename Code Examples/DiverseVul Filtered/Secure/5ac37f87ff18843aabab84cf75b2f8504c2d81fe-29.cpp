static inline void native_store_idt(struct desc_ptr *dtr)
{
	asm volatile("sidt %0":"=m" (*dtr));
}