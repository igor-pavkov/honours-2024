static void twaddle(int fdc, int drive)
{
	if (drive_params[drive].select_delay)
		return;
	fdc_outb(fdc_state[fdc].dor & ~(0x10 << UNIT(drive)),
		 fdc, FD_DOR);
	fdc_outb(fdc_state[fdc].dor, fdc, FD_DOR);
	drive_state[drive].select_date = jiffies;
}