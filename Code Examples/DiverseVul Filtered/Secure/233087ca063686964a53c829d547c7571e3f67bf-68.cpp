static void request_done(int uptodate)
{
	struct request *req = current_req;
	int block;
	char msg[sizeof("request done ") + sizeof(int) * 3];

	probing = 0;
	snprintf(msg, sizeof(msg), "request done %d", uptodate);
	reschedule_timeout(MAXTIMEOUT, msg);

	if (!req) {
		pr_info("floppy.c: no request in request_done\n");
		return;
	}

	if (uptodate) {
		/* maintain values for invalidation on geometry
		 * change */
		block = current_count_sectors + blk_rq_pos(req);
		INFBOUND(drive_state[current_drive].maxblock, block);
		if (block > _floppy->sect)
			drive_state[current_drive].maxtrack = 1;

		floppy_end_request(req, 0);
	} else {
		if (rq_data_dir(req) == WRITE) {
			/* record write error information */
			write_errors[current_drive].write_errors++;
			if (write_errors[current_drive].write_errors == 1) {
				write_errors[current_drive].first_error_sector = blk_rq_pos(req);
				write_errors[current_drive].first_error_generation = drive_state[current_drive].generation;
			}
			write_errors[current_drive].last_error_sector = blk_rq_pos(req);
			write_errors[current_drive].last_error_generation = drive_state[current_drive].generation;
		}
		floppy_end_request(req, BLK_STS_IOERR);
	}
}