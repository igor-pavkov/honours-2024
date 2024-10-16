static inline void set_tssldt_descriptor(void *d, unsigned long addr,
					 unsigned type, unsigned size)
{
#ifdef CONFIG_X86_64
	struct ldttss_desc64 *desc = d;
	memset(desc, 0, sizeof(*desc));
	desc->limit0 = size & 0xFFFF;
	desc->base0 = PTR_LOW(addr);
	desc->base1 = PTR_MIDDLE(addr) & 0xFF;
	desc->type = type;
	desc->p = 1;
	desc->limit1 = (size >> 16) & 0xF;
	desc->base2 = (PTR_MIDDLE(addr) >> 8) & 0xFF;
	desc->base3 = PTR_HIGH(addr);
#else
	pack_descriptor((struct desc_struct *)d, addr, size, 0x80 | type, 0);
#endif
}