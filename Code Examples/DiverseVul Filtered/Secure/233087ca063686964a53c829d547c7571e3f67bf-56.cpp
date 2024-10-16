static void setup_DMA(void)
{
	unsigned long f;

	if (raw_cmd->length == 0) {
		print_hex_dump(KERN_INFO, "zero dma transfer size: ",
			       DUMP_PREFIX_NONE, 16, 1,
			       raw_cmd->fullcmd, raw_cmd->cmd_count, false);
		cont->done(0);
		fdc_state[current_fdc].reset = 1;
		return;
	}
	if (((unsigned long)raw_cmd->kernel_data) % 512) {
		pr_info("non aligned address: %p\n", raw_cmd->kernel_data);
		cont->done(0);
		fdc_state[current_fdc].reset = 1;
		return;
	}
	f = claim_dma_lock();
	fd_disable_dma();
#ifdef fd_dma_setup
	if (fd_dma_setup(raw_cmd->kernel_data, raw_cmd->length,
			 (raw_cmd->flags & FD_RAW_READ) ?
			 DMA_MODE_READ : DMA_MODE_WRITE,
			 fdc_state[current_fdc].address) < 0) {
		release_dma_lock(f);
		cont->done(0);
		fdc_state[current_fdc].reset = 1;
		return;
	}
	release_dma_lock(f);
#else
	fd_clear_dma_ff();
	fd_cacheflush(raw_cmd->kernel_data, raw_cmd->length);
	fd_set_dma_mode((raw_cmd->flags & FD_RAW_READ) ?
			DMA_MODE_READ : DMA_MODE_WRITE);
	fd_set_dma_addr(raw_cmd->kernel_data);
	fd_set_dma_count(raw_cmd->length);
	virtual_dma_port = fdc_state[current_fdc].address;
	fd_enable_dma();
	release_dma_lock(f);
#endif
}