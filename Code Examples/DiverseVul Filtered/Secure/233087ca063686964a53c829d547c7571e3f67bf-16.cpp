static int fdc_configure(int fdc)
{
	/* Turn on FIFO */
	output_byte(fdc, FD_CONFIGURE);
	if (need_more_output(fdc) != MORE_OUTPUT)
		return 0;
	output_byte(fdc, 0);
	output_byte(fdc, 0x10 | (no_fifo & 0x20) | (fifo_depth & 0xf));
	output_byte(fdc, 0);    /* pre-compensation from track 0 upwards */
	return 1;
}