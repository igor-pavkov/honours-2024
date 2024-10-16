static inline void smb2_sg_set_buf(struct scatterlist *sg, const void *buf,
				   unsigned int buflen)
{
	void *addr;
	/*
	 * VMAP_STACK (at least) puts stack into the vmalloc address space
	 */
	if (is_vmalloc_addr(buf))
		addr = vmalloc_to_page(buf);
	else
		addr = virt_to_page(buf);
	sg_set_page(sg, addr, buflen, offset_in_page(buf));
}