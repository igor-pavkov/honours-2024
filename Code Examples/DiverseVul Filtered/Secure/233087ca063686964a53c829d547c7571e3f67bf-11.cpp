static void reset_fdc_info(int fdc, int mode)
{
	int drive;

	fdc_state[fdc].spec1 = fdc_state[fdc].spec2 = -1;
	fdc_state[fdc].need_configure = 1;
	fdc_state[fdc].perp_mode = 1;
	fdc_state[fdc].rawcmd = 0;
	for (drive = 0; drive < N_DRIVE; drive++)
		if (FDC(drive) == fdc &&
		    (mode || drive_state[drive].track != NEED_1_RECAL))
			drive_state[drive].track = NEED_2_RECAL;
}