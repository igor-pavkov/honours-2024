static void recalibrate_floppy(void)
{
	debugt(__func__, "");
	do_floppy = recal_interrupt;
	output_byte(current_fdc, FD_RECALIBRATE);
	if (output_byte(current_fdc, UNIT(current_drive)) < 0)
		reset_fdc();
}