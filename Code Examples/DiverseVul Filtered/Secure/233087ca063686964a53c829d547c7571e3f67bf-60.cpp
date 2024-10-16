static int floppy_grab_irq_and_dma(void)
{
	int fdc;

	if (atomic_inc_return(&usage_count) > 1)
		return 0;

	/*
	 * We might have scheduled a free_irq(), wait it to
	 * drain first:
	 */
	flush_workqueue(floppy_wq);

	if (fd_request_irq()) {
		DPRINT("Unable to grab IRQ%d for the floppy driver\n",
		       FLOPPY_IRQ);
		atomic_dec(&usage_count);
		return -1;
	}
	if (fd_request_dma()) {
		DPRINT("Unable to grab DMA%d for the floppy driver\n",
		       FLOPPY_DMA);
		if (can_use_virtual_dma & 2)
			use_virtual_dma = can_use_virtual_dma = 1;
		if (!(can_use_virtual_dma & 1)) {
			fd_free_irq();
			atomic_dec(&usage_count);
			return -1;
		}
	}

	for (fdc = 0; fdc < N_FDC; fdc++) {
		if (fdc_state[fdc].address != -1) {
			if (floppy_request_regions(fdc))
				goto cleanup;
		}
	}
	for (fdc = 0; fdc < N_FDC; fdc++) {
		if (fdc_state[fdc].address != -1) {
			reset_fdc_info(fdc, 1);
			fdc_outb(fdc_state[fdc].dor, fdc, FD_DOR);
		}
	}

	set_dor(0, ~0, 8);	/* avoid immediate interrupt */

	for (fdc = 0; fdc < N_FDC; fdc++)
		if (fdc_state[fdc].address != -1)
			fdc_outb(fdc_state[fdc].dor, fdc, FD_DOR);
	/*
	 * The driver will try and free resources and relies on us
	 * to know if they were allocated or not.
	 */
	current_fdc = 0;
	irqdma_allocated = 1;
	return 0;
cleanup:
	fd_free_irq();
	fd_free_dma();
	while (--fdc >= 0)
		floppy_release_regions(fdc);
	current_fdc = 0;
	atomic_dec(&usage_count);
	return -1;
}