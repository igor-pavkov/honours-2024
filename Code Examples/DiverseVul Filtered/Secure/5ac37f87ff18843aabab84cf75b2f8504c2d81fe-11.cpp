static inline void native_write_gdt_entry(struct desc_struct *gdt, int entry,
					  const void *desc, int type)
{
	unsigned int size;
	switch (type) {
	case DESC_TSS:
		size = sizeof(tss_desc);
		break;
	case DESC_LDT:
		size = sizeof(ldt_desc);
		break;
	default:
		size = sizeof(struct desc_struct);
		break;
	}
	memcpy(&gdt[entry], desc, size);
}