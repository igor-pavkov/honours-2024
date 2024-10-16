static void setup_format_params(int track)
{
	int n;
	int il;
	int count;
	int head_shift;
	int track_shift;
	struct fparm {
		unsigned char track, head, sect, size;
	} *here = (struct fparm *)floppy_track_buffer;

	raw_cmd = &default_raw_cmd;
	raw_cmd->track = track;

	raw_cmd->flags = (FD_RAW_WRITE | FD_RAW_INTR | FD_RAW_SPIN |
			  FD_RAW_NEED_DISK | FD_RAW_NEED_SEEK);
	raw_cmd->rate = _floppy->rate & 0x43;
	raw_cmd->cmd_count = NR_F;
	raw_cmd->cmd[COMMAND] = FM_MODE(_floppy, FD_FORMAT);
	raw_cmd->cmd[DR_SELECT] = UNIT(current_drive) + PH_HEAD(_floppy, format_req.head);
	raw_cmd->cmd[F_SIZECODE] = FD_SIZECODE(_floppy);
	raw_cmd->cmd[F_SECT_PER_TRACK] = _floppy->sect << 2 >> raw_cmd->cmd[F_SIZECODE];
	raw_cmd->cmd[F_GAP] = _floppy->fmt_gap;
	raw_cmd->cmd[F_FILL] = FD_FILL_BYTE;

	raw_cmd->kernel_data = floppy_track_buffer;
	raw_cmd->length = 4 * raw_cmd->cmd[F_SECT_PER_TRACK];

	if (!raw_cmd->cmd[F_SECT_PER_TRACK])
		return;

	/* allow for about 30ms for data transport per track */
	head_shift = (raw_cmd->cmd[F_SECT_PER_TRACK] + 5) / 6;

	/* a ``cylinder'' is two tracks plus a little stepping time */
	track_shift = 2 * head_shift + 3;

	/* position of logical sector 1 on this track */
	n = (track_shift * format_req.track + head_shift * format_req.head)
	    % raw_cmd->cmd[F_SECT_PER_TRACK];

	/* determine interleave */
	il = 1;
	if (_floppy->fmt_gap < 0x22)
		il++;

	/* initialize field */
	for (count = 0; count < raw_cmd->cmd[F_SECT_PER_TRACK]; ++count) {
		here[count].track = format_req.track;
		here[count].head = format_req.head;
		here[count].sect = 0;
		here[count].size = raw_cmd->cmd[F_SIZECODE];
	}
	/* place logical sectors */
	for (count = 1; count <= raw_cmd->cmd[F_SECT_PER_TRACK]; ++count) {
		here[n].sect = count;
		n = (n + il) % raw_cmd->cmd[F_SECT_PER_TRACK];
		if (here[n].sect) {	/* sector busy, find next free sector */
			++n;
			if (n >= raw_cmd->cmd[F_SECT_PER_TRACK]) {
				n -= raw_cmd->cmd[F_SECT_PER_TRACK];
				while (here[n].sect)
					++n;
			}
		}
	}
	if (_floppy->stretch & FD_SECTBASEMASK) {
		for (count = 0; count < raw_cmd->cmd[F_SECT_PER_TRACK]; count++)
			here[count].sect += FD_SECTBASE(_floppy) - 1;
	}
}