static inline void native_write_ldt_entry(struct desc_struct *ldt, int entry,
					  const void *desc)
{
	memcpy(&ldt[entry], desc, 8);
}