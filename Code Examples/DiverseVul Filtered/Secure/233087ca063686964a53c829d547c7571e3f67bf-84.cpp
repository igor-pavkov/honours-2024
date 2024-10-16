static int output_byte(int fdc, char byte)
{
	int status = wait_til_ready(fdc);

	if (status < 0)
		return -1;

	if (is_ready_state(status)) {
		fdc_outb(byte, fdc, FD_DATA);
		output_log[output_log_pos].data = byte;
		output_log[output_log_pos].status = status;
		output_log[output_log_pos].jiffies = jiffies;
		output_log_pos = (output_log_pos + 1) % OLOGSIZE;
		return 0;
	}
	fdc_state[fdc].reset = 1;
	if (initialized) {
		DPRINT("Unable to send byte %x to FDC. Fdc=%x Status=%x\n",
		       byte, fdc, status);
		show_floppy(fdc);
	}
	return -1;
}