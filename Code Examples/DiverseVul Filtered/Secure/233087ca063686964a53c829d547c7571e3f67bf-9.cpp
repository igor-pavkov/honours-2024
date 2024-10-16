static int fdc_dtr(void)
{
	/* If data rate not already set to desired value, set it. */
	if ((raw_cmd->rate & 3) == fdc_state[current_fdc].dtr)
		return 0;

	/* Set dtr */
	fdc_outb(raw_cmd->rate & 3, current_fdc, FD_DCR);

	/* TODO: some FDC/drive combinations (C&T 82C711 with TEAC 1.2MB)
	 * need a stabilization period of several milliseconds to be
	 * enforced after data rate changes before R/W operations.
	 * Pause 5 msec to avoid trouble. (Needs to be 2 jiffies)
	 */
	fdc_state[current_fdc].dtr = raw_cmd->rate & 3;
	return fd_wait_for_completion(jiffies + 2UL * HZ / 100, floppy_ready);
}				/* fdc_dtr */