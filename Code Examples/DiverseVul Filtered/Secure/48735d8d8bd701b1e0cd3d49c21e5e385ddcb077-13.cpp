static inline void clear_bss_descriptor(struct bss_descriptor *bss)
{
	/* Don't blow away ->list, just BSS data */
	memset(bss, 0, offsetof(struct bss_descriptor, list));
}