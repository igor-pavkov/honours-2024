static int disk_change(int drive)
{
	int fdc = FDC(drive);

	if (time_before(jiffies, drive_state[drive].select_date + drive_params[drive].select_delay))
		DPRINT("WARNING disk change called early\n");
	if (!(fdc_state[fdc].dor & (0x10 << UNIT(drive))) ||
	    (fdc_state[fdc].dor & 3) != UNIT(drive) || fdc != FDC(drive)) {
		DPRINT("probing disk change on unselected drive\n");
		DPRINT("drive=%d fdc=%d dor=%x\n", drive, FDC(drive),
		       (unsigned int)fdc_state[fdc].dor);
	}

	debug_dcl(drive_params[drive].flags,
		  "checking disk change line for drive %d\n", drive);
	debug_dcl(drive_params[drive].flags, "jiffies=%lu\n", jiffies);
	debug_dcl(drive_params[drive].flags, "disk change line=%x\n",
		  fdc_inb(fdc, FD_DIR) & 0x80);
	debug_dcl(drive_params[drive].flags, "flags=%lx\n",
		  drive_state[drive].flags);

	if (drive_params[drive].flags & FD_BROKEN_DCL)
		return test_bit(FD_DISK_CHANGED_BIT,
				&drive_state[drive].flags);
	if ((fdc_inb(fdc, FD_DIR) ^ drive_params[drive].flags) & 0x80) {
		set_bit(FD_VERIFY_BIT, &drive_state[drive].flags);
					/* verify write protection */

		if (drive_state[drive].maxblock)	/* mark it changed */
			set_bit(FD_DISK_CHANGED_BIT,
				&drive_state[drive].flags);

		/* invalidate its geometry */
		if (drive_state[drive].keep_data >= 0) {
			if ((drive_params[drive].flags & FTD_MSG) &&
			    current_type[drive] != NULL)
				DPRINT("Disk type is undefined after disk change\n");
			current_type[drive] = NULL;
			floppy_sizes[TOMINOR(drive)] = MAX_DISK_SIZE << 1;
		}

		return 1;
	} else {
		drive_state[drive].last_checked = jiffies;
		clear_bit(FD_DISK_NEWCHANGE_BIT, &drive_state[drive].flags);
	}
	return 0;
}