static int set_dor(int fdc, char mask, char data)
{
	unsigned char unit;
	unsigned char drive;
	unsigned char newdor;
	unsigned char olddor;

	if (fdc_state[fdc].address == -1)
		return -1;

	olddor = fdc_state[fdc].dor;
	newdor = (olddor & mask) | data;
	if (newdor != olddor) {
		unit = olddor & 0x3;
		if (is_selected(olddor, unit) && !is_selected(newdor, unit)) {
			drive = REVDRIVE(fdc, unit);
			debug_dcl(drive_params[drive].flags,
				  "calling disk change from set_dor\n");
			disk_change(drive);
		}
		fdc_state[fdc].dor = newdor;
		fdc_outb(newdor, fdc, FD_DOR);

		unit = newdor & 0x3;
		if (!is_selected(olddor, unit) && is_selected(newdor, unit)) {
			drive = REVDRIVE(fdc, unit);
			drive_state[drive].select_date = jiffies;
		}
	}
	return olddor;
}