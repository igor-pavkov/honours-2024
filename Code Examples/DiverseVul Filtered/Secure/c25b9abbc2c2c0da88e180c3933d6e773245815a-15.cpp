void *mac_drv_get_desc_mem(struct s_smc *smc, unsigned int size)
{

	char *virt;

	PRINTK(KERN_INFO "mac_drv_get_desc_mem\n");

	// Descriptor memory must be aligned on 16-byte boundary.

	virt = mac_drv_get_space(smc, size);

	size = (u_int) (16 - (((unsigned long) virt) & 15UL));
	size = size % 16;

	PRINTK("Allocate %u bytes alignment gap ", size);
	PRINTK("for descriptor memory.\n");

	if (!mac_drv_get_space(smc, size)) {
		printk("fddi: Unable to align descriptor memory.\n");
		return (NULL);
	}
	return (virt + size);
}				// mac_drv_get_desc_mem