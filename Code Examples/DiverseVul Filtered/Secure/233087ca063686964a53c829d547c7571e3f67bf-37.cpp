static int make_raw_rw_request(void)
{
	int aligned_sector_t;
	int max_sector;
	int max_size;
	int tracksize;
	int ssize;

	if (WARN(max_buffer_sectors == 0, "VFS: Block I/O scheduled on unopened device\n"))
		return 0;

	set_fdc((long)current_req->q->disk->private_data);

	raw_cmd = &default_raw_cmd;
	raw_cmd->flags = FD_RAW_SPIN | FD_RAW_NEED_DISK | FD_RAW_NEED_SEEK;
	raw_cmd->cmd_count = NR_RW;
	if (rq_data_dir(current_req) == READ) {
		raw_cmd->flags |= FD_RAW_READ;
		raw_cmd->cmd[COMMAND] = FM_MODE(_floppy, FD_READ);
	} else if (rq_data_dir(current_req) == WRITE) {
		raw_cmd->flags |= FD_RAW_WRITE;
		raw_cmd->cmd[COMMAND] = FM_MODE(_floppy, FD_WRITE);
	} else {
		DPRINT("%s: unknown command\n", __func__);
		return 0;
	}

	max_sector = _floppy->sect * _floppy->head;

	raw_cmd->cmd[TRACK] = (int)blk_rq_pos(current_req) / max_sector;
	fsector_t = (int)blk_rq_pos(current_req) % max_sector;
	if (_floppy->track && raw_cmd->cmd[TRACK] >= _floppy->track) {
		if (blk_rq_cur_sectors(current_req) & 1) {
			current_count_sectors = 1;
			return 1;
		} else
			return 0;
	}
	raw_cmd->cmd[HEAD] = fsector_t / _floppy->sect;

	if (((_floppy->stretch & (FD_SWAPSIDES | FD_SECTBASEMASK)) ||
	     test_bit(FD_NEED_TWADDLE_BIT, &drive_state[current_drive].flags)) &&
	    fsector_t < _floppy->sect)
		max_sector = _floppy->sect;

	/* 2M disks have phantom sectors on the first track */
	if ((_floppy->rate & FD_2M) && (!raw_cmd->cmd[TRACK]) && (!raw_cmd->cmd[HEAD])) {
		max_sector = 2 * _floppy->sect / 3;
		if (fsector_t >= max_sector) {
			current_count_sectors =
			    min_t(int, _floppy->sect - fsector_t,
				  blk_rq_sectors(current_req));
			return 1;
		}
		raw_cmd->cmd[SIZECODE] = 2;
	} else
		raw_cmd->cmd[SIZECODE] = FD_SIZECODE(_floppy);
	raw_cmd->rate = _floppy->rate & 0x43;
	if ((_floppy->rate & FD_2M) &&
	    (raw_cmd->cmd[TRACK] || raw_cmd->cmd[HEAD]) && raw_cmd->rate == 2)
		raw_cmd->rate = 1;

	if (raw_cmd->cmd[SIZECODE])
		raw_cmd->cmd[SIZECODE2] = 0xff;
	else
		raw_cmd->cmd[SIZECODE2] = 0x80;
	raw_cmd->track = raw_cmd->cmd[TRACK] << STRETCH(_floppy);
	raw_cmd->cmd[DR_SELECT] = UNIT(current_drive) + PH_HEAD(_floppy, raw_cmd->cmd[HEAD]);
	raw_cmd->cmd[GAP] = _floppy->gap;
	ssize = DIV_ROUND_UP(1 << raw_cmd->cmd[SIZECODE], 4);
	raw_cmd->cmd[SECT_PER_TRACK] = _floppy->sect << 2 >> raw_cmd->cmd[SIZECODE];
	raw_cmd->cmd[SECTOR] = ((fsector_t % _floppy->sect) << 2 >> raw_cmd->cmd[SIZECODE]) +
	    FD_SECTBASE(_floppy);

	/* tracksize describes the size which can be filled up with sectors
	 * of size ssize.
	 */
	tracksize = _floppy->sect - _floppy->sect % ssize;
	if (tracksize < _floppy->sect) {
		raw_cmd->cmd[SECT_PER_TRACK]++;
		if (tracksize <= fsector_t % _floppy->sect)
			raw_cmd->cmd[SECTOR]--;

		/* if we are beyond tracksize, fill up using smaller sectors */
		while (tracksize <= fsector_t % _floppy->sect) {
			while (tracksize + ssize > _floppy->sect) {
				raw_cmd->cmd[SIZECODE]--;
				ssize >>= 1;
			}
			raw_cmd->cmd[SECTOR]++;
			raw_cmd->cmd[SECT_PER_TRACK]++;
			tracksize += ssize;
		}
		max_sector = raw_cmd->cmd[HEAD] * _floppy->sect + tracksize;
	} else if (!raw_cmd->cmd[TRACK] && !raw_cmd->cmd[HEAD] && !(_floppy->rate & FD_2M) && probing) {
		max_sector = _floppy->sect;
	} else if (!raw_cmd->cmd[HEAD] && CT(raw_cmd->cmd[COMMAND]) == FD_WRITE) {
		/* for virtual DMA bug workaround */
		max_sector = _floppy->sect;
	}

	in_sector_offset = (fsector_t % _floppy->sect) % ssize;
	aligned_sector_t = fsector_t - in_sector_offset;
	max_size = blk_rq_sectors(current_req);
	if ((raw_cmd->track == buffer_track) &&
	    (current_drive == buffer_drive) &&
	    (fsector_t >= buffer_min) && (fsector_t < buffer_max)) {
		/* data already in track buffer */
		if (CT(raw_cmd->cmd[COMMAND]) == FD_READ) {
			copy_buffer(1, max_sector, buffer_max);
			return 1;
		}
	} else if (in_sector_offset || blk_rq_sectors(current_req) < ssize) {
		if (CT(raw_cmd->cmd[COMMAND]) == FD_WRITE) {
			unsigned int sectors;

			sectors = fsector_t + blk_rq_sectors(current_req);
			if (sectors > ssize && sectors < ssize + ssize)
				max_size = ssize + ssize;
			else
				max_size = ssize;
		}
		raw_cmd->flags &= ~FD_RAW_WRITE;
		raw_cmd->flags |= FD_RAW_READ;
		raw_cmd->cmd[COMMAND] = FM_MODE(_floppy, FD_READ);
	}

	if (CT(raw_cmd->cmd[COMMAND]) == FD_READ)
		max_size = max_sector;	/* unbounded */

	/* claim buffer track if needed */
	if (buffer_track != raw_cmd->track ||	/* bad track */
	    buffer_drive != current_drive ||	/* bad drive */
	    fsector_t > buffer_max ||
	    fsector_t < buffer_min ||
	    ((CT(raw_cmd->cmd[COMMAND]) == FD_READ ||
	      (!in_sector_offset && blk_rq_sectors(current_req) >= ssize)) &&
	     max_sector > 2 * max_buffer_sectors + buffer_min &&
	     max_size + fsector_t > 2 * max_buffer_sectors + buffer_min)) {
		/* not enough space */
		buffer_track = -1;
		buffer_drive = current_drive;
		buffer_max = buffer_min = aligned_sector_t;
	}
	raw_cmd->kernel_data = floppy_track_buffer +
		((aligned_sector_t - buffer_min) << 9);

	if (CT(raw_cmd->cmd[COMMAND]) == FD_WRITE) {
		/* copy write buffer to track buffer.
		 * if we get here, we know that the write
		 * is either aligned or the data already in the buffer
		 * (buffer will be overwritten) */
		if (in_sector_offset && buffer_track == -1)
			DPRINT("internal error offset !=0 on write\n");
		buffer_track = raw_cmd->track;
		buffer_drive = current_drive;
		copy_buffer(ssize, max_sector,
			    2 * max_buffer_sectors + buffer_min);
	} else
		transfer_size(ssize, max_sector,
			      2 * max_buffer_sectors + buffer_min -
			      aligned_sector_t);

	/* round up current_count_sectors to get dma xfer size */
	raw_cmd->length = in_sector_offset + current_count_sectors;
	raw_cmd->length = ((raw_cmd->length - 1) | (ssize - 1)) + 1;
	raw_cmd->length <<= 9;
	if ((raw_cmd->length < current_count_sectors << 9) ||
	    (CT(raw_cmd->cmd[COMMAND]) == FD_WRITE &&
	     (aligned_sector_t + (raw_cmd->length >> 9) > buffer_max ||
	      aligned_sector_t < buffer_min)) ||
	    raw_cmd->length % (128 << raw_cmd->cmd[SIZECODE]) ||
	    raw_cmd->length <= 0 || current_count_sectors <= 0) {
		DPRINT("fractionary current count b=%lx s=%lx\n",
		       raw_cmd->length, current_count_sectors);
		pr_info("addr=%d, length=%ld\n",
			(int)((raw_cmd->kernel_data -
			       floppy_track_buffer) >> 9),
			current_count_sectors);
		pr_info("st=%d ast=%d mse=%d msi=%d\n",
			fsector_t, aligned_sector_t, max_sector, max_size);
		pr_info("ssize=%x SIZECODE=%d\n", ssize, raw_cmd->cmd[SIZECODE]);
		pr_info("command=%x SECTOR=%d HEAD=%d, TRACK=%d\n",
			raw_cmd->cmd[COMMAND], raw_cmd->cmd[SECTOR],
			raw_cmd->cmd[HEAD], raw_cmd->cmd[TRACK]);
		pr_info("buffer drive=%d\n", buffer_drive);
		pr_info("buffer track=%d\n", buffer_track);
		pr_info("buffer_min=%d\n", buffer_min);
		pr_info("buffer_max=%d\n", buffer_max);
		return 0;
	}

	if (raw_cmd->kernel_data < floppy_track_buffer ||
	    current_count_sectors < 0 ||
	    raw_cmd->length < 0 ||
	    raw_cmd->kernel_data + raw_cmd->length >
	    floppy_track_buffer + (max_buffer_sectors << 10)) {
		DPRINT("buffer overrun in schedule dma\n");
		pr_info("fsector_t=%d buffer_min=%d current_count=%ld\n",
			fsector_t, buffer_min, raw_cmd->length >> 9);
		pr_info("current_count_sectors=%ld\n",
			current_count_sectors);
		if (CT(raw_cmd->cmd[COMMAND]) == FD_READ)
			pr_info("read\n");
		if (CT(raw_cmd->cmd[COMMAND]) == FD_WRITE)
			pr_info("write\n");
		return 0;
	}
	if (raw_cmd->length == 0) {
		DPRINT("zero dma transfer attempted from make_raw_request\n");
		return 0;
	}

	virtualdmabug_workaround();
	return 2;
}