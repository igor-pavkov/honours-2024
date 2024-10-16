static void perpendicular_mode(int fdc)
{
	unsigned char perp_mode;

	if (raw_cmd->rate & 0x40) {
		switch (raw_cmd->rate & 3) {
		case 0:
			perp_mode = 2;
			break;
		case 3:
			perp_mode = 3;
			break;
		default:
			DPRINT("Invalid data rate for perpendicular mode!\n");
			cont->done(0);
			fdc_state[fdc].reset = 1;
					/*
					 * convenient way to return to
					 * redo without too much hassle
					 * (deep stack et al.)
					 */
			return;
		}
	} else
		perp_mode = 0;

	if (fdc_state[fdc].perp_mode == perp_mode)
		return;
	if (fdc_state[fdc].version >= FDC_82077_ORIG) {
		output_byte(fdc, FD_PERPENDICULAR);
		output_byte(fdc, perp_mode);
		fdc_state[fdc].perp_mode = perp_mode;
	} else if (perp_mode) {
		DPRINT("perpendicular mode not supported by this FDC.\n");
	}
}				/* perpendicular_mode */