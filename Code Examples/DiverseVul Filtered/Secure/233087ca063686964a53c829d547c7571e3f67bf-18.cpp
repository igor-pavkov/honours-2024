static int result(int fdc)
{
	int i;
	int status = 0;

	for (i = 0; i < FD_RAW_REPLY_SIZE; i++) {
		status = wait_til_ready(fdc);
		if (status < 0)
			break;
		status &= STATUS_DIR | STATUS_READY | STATUS_BUSY | STATUS_DMA;
		if ((status & ~STATUS_BUSY) == STATUS_READY) {
			resultjiffies = jiffies;
			resultsize = i;
			return i;
		}
		if (status == (STATUS_DIR | STATUS_READY | STATUS_BUSY))
			reply_buffer[i] = fdc_inb(fdc, FD_DATA);
		else
			break;
	}
	if (initialized) {
		DPRINT("get result error. Fdc=%d Last status=%x Read bytes=%d\n",
		       fdc, status, i);
		show_floppy(fdc);
	}
	fdc_state[fdc].reset = 1;
	return -1;
}