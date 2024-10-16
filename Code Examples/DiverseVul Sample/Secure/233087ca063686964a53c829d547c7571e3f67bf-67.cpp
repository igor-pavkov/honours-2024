static void reset_fdc(void)
{
	unsigned long flags;

	do_floppy = reset_interrupt;
	fdc_state[current_fdc].reset = 0;
	reset_fdc_info(current_fdc, 0);

	/* Pseudo-DMA may intercept 'reset finished' interrupt.  */
	/* Irrelevant for systems with true DMA (i386).          */

	flags = claim_dma_lock();
	fd_disable_dma();
	release_dma_lock(flags);

	if (fdc_state[current_fdc].version >= FDC_82072A)
		fdc_outb(0x80 | (fdc_state[current_fdc].dtr & 3),
			 current_fdc, FD_STATUS);
	else {
		fdc_outb(fdc_state[current_fdc].dor & ~0x04, current_fdc, FD_DOR);
		udelay(FD_RESET_DELAY);
		fdc_outb(fdc_state[current_fdc].dor, current_fdc, FD_DOR);
	}
}