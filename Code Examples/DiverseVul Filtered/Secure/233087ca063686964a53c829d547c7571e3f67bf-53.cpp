static void rw_interrupt(void)
{
	int eoc;
	int ssize;
	int heads;
	int nr_sectors;

	if (reply_buffer[R_HEAD] >= 2) {
		/* some Toshiba floppy controllers occasionnally seem to
		 * return bogus interrupts after read/write operations, which
		 * can be recognized by a bad head number (>= 2) */
		return;
	}

	if (!drive_state[current_drive].first_read_date)
		drive_state[current_drive].first_read_date = jiffies;

	ssize = DIV_ROUND_UP(1 << raw_cmd->cmd[SIZECODE], 4);

	if (reply_buffer[ST1] & ST1_EOC)
		eoc = 1;
	else
		eoc = 0;

	if (raw_cmd->cmd[COMMAND] & 0x80)
		heads = 2;
	else
		heads = 1;

	nr_sectors = (((reply_buffer[R_TRACK] - raw_cmd->cmd[TRACK]) * heads +
		       reply_buffer[R_HEAD] - raw_cmd->cmd[HEAD]) * raw_cmd->cmd[SECT_PER_TRACK] +
		      reply_buffer[R_SECTOR] - raw_cmd->cmd[SECTOR] + eoc) << raw_cmd->cmd[SIZECODE] >> 2;

	if (nr_sectors / ssize >
	    DIV_ROUND_UP(in_sector_offset + current_count_sectors, ssize)) {
		DPRINT("long rw: %x instead of %lx\n",
		       nr_sectors, current_count_sectors);
		pr_info("rs=%d s=%d\n", reply_buffer[R_SECTOR],
			raw_cmd->cmd[SECTOR]);
		pr_info("rh=%d h=%d\n", reply_buffer[R_HEAD],
			raw_cmd->cmd[HEAD]);
		pr_info("rt=%d t=%d\n", reply_buffer[R_TRACK],
			raw_cmd->cmd[TRACK]);
		pr_info("heads=%d eoc=%d\n", heads, eoc);
		pr_info("spt=%d st=%d ss=%d\n",
			raw_cmd->cmd[SECT_PER_TRACK], fsector_t, ssize);
		pr_info("in_sector_offset=%d\n", in_sector_offset);
	}

	nr_sectors -= in_sector_offset;
	INFBOUND(nr_sectors, 0);
	SUPBOUND(current_count_sectors, nr_sectors);

	switch (interpret_errors()) {
	case 2:
		cont->redo();
		return;
	case 1:
		if (!current_count_sectors) {
			cont->error();
			cont->redo();
			return;
		}
		break;
	case 0:
		if (!current_count_sectors) {
			cont->redo();
			return;
		}
		current_type[current_drive] = _floppy;
		floppy_sizes[TOMINOR(current_drive)] = _floppy->size;
		break;
	}

	if (probing) {
		if (drive_params[current_drive].flags & FTD_MSG)
			DPRINT("Auto-detected floppy type %s in fd%d\n",
			       _floppy->name, current_drive);
		current_type[current_drive] = _floppy;
		floppy_sizes[TOMINOR(current_drive)] = _floppy->size;
		probing = 0;
	}

	if (CT(raw_cmd->cmd[COMMAND]) != FD_READ) {
		/* transfer directly from buffer */
		cont->done(1);
	} else {
		buffer_track = raw_cmd->track;
		buffer_drive = current_drive;
		INFBOUND(buffer_max, nr_sectors + fsector_t);
	}
	cont->redo();
}