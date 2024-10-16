unsigned long mac_drv_virt2phys(struct s_smc *smc, void *virt)
{
	return (smc->os.SharedMemDMA +
		((char *) virt - (char *)smc->os.SharedMemAddr));
}				// mac_drv_virt2phys