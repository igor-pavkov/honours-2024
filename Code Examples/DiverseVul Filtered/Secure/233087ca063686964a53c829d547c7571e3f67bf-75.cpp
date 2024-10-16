static void floppy_shutdown(struct work_struct *arg)
{
	unsigned long flags;

	if (initialized)
		show_floppy(current_fdc);
	cancel_activity();

	flags = claim_dma_lock();
	fd_disable_dma();
	release_dma_lock(flags);

	/* avoid dma going to a random drive after shutdown */

	if (initialized)
		DPRINT("floppy timeout called\n");
	fdc_state[current_fdc].reset = 1;
	if (cont) {
		cont->done(0);
		cont->redo();	/* this will recall reset when needed */
	} else {
		pr_info("no cont in shutdown!\n");
		process_fd_request();
	}
	is_alive(__func__, "");
}