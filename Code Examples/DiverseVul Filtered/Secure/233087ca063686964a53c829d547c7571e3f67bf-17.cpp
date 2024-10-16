static int wait_til_ready(int fdc)
{
	int status;
	int counter;

	if (fdc_state[fdc].reset)
		return -1;
	for (counter = 0; counter < 10000; counter++) {
		status = fdc_inb(fdc, FD_STATUS);
		if (status & STATUS_READY)
			return status;
	}
	if (initialized) {
		DPRINT("Getstatus times out (%x) on fdc %d\n", status, fdc);
		show_floppy(fdc);
	}
	fdc_state[fdc].reset = 1;
	return -1;
}