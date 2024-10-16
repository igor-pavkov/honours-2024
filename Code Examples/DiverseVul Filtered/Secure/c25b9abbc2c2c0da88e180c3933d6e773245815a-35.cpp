void *mac_drv_get_space(struct s_smc *smc, unsigned int size)
{
	void *virt;

	PRINTK(KERN_INFO "mac_drv_get_space (%d bytes), ", size);
	virt = (void *) (smc->os.SharedMemAddr + smc->os.SharedMemHeap);

	if ((smc->os.SharedMemHeap + size) > smc->os.SharedMemSize) {
		printk("Unexpected SMT memory size requested: %d\n", size);
		return (NULL);
	}
	smc->os.SharedMemHeap += size;	// Move heap pointer.

	PRINTK(KERN_INFO "mac_drv_get_space end\n");
	PRINTK(KERN_INFO "virt addr: %lx\n", (ulong) virt);
	PRINTK(KERN_INFO "bus  addr: %lx\n", (ulong)
	       (smc->os.SharedMemDMA +
		((char *) virt - (char *)smc->os.SharedMemAddr)));
	return (virt);
}				// mac_drv_get_space