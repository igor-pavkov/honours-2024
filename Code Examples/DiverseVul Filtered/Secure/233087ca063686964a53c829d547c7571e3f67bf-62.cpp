static blk_status_t floppy_queue_rq(struct blk_mq_hw_ctx *hctx,
				    const struct blk_mq_queue_data *bd)
{
	blk_mq_start_request(bd->rq);

	if (WARN(max_buffer_sectors == 0,
		 "VFS: %s called on non-open device\n", __func__))
		return BLK_STS_IOERR;

	if (WARN(atomic_read(&usage_count) == 0,
		 "warning: usage count=0, current_req=%p sect=%ld flags=%llx\n",
		 current_req, (long)blk_rq_pos(current_req),
		 (unsigned long long) current_req->cmd_flags))
		return BLK_STS_IOERR;

	if (test_and_set_bit(0, &fdc_busy)) {
		/* fdc busy, this new request will be treated when the
		   current one is done */
		is_alive(__func__, "old request running");
		return BLK_STS_RESOURCE;
	}

	spin_lock_irq(&floppy_lock);
	list_add_tail(&bd->rq->queuelist, &floppy_reqs);
	spin_unlock_irq(&floppy_lock);

	command_status = FD_COMMAND_NONE;
	__reschedule_timeout(MAXTIMEOUT, "fd_request");
	set_fdc(0);
	process_fd_request();
	is_alive(__func__, "");
	return BLK_STS_OK;
}