u_long dma_master(struct s_smc * smc, void *virt, int len, int flag)
{
	return (smc->os.SharedMemDMA +
		((char *) virt - (char *)smc->os.SharedMemAddr));
}				// dma_master