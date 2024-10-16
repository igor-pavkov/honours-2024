static inline void __set_tss_desc(unsigned cpu, unsigned int entry, void *addr)
{
	struct desc_struct *d = get_cpu_gdt_table(cpu);
	tss_desc tss;

	/*
	 * sizeof(unsigned long) coming from an extra "long" at the end
	 * of the iobitmap. See tss_struct definition in processor.h
	 *
	 * -1? seg base+limit should be pointing to the address of the
	 * last valid byte
	 */
	set_tssldt_descriptor(&tss, (unsigned long)addr, DESC_TSS,
			      IO_BITMAP_OFFSET + IO_BITMAP_BYTES +
			      sizeof(unsigned long) - 1);
	write_gdt_entry(d, entry, &tss, DESC_TSS);
}