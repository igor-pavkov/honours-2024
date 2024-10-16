static int cdrom_read_cdda_bpc(struct cdrom_device_info *cdi, __u8 __user *ubuf,
			       int lba, int nframes)
{
	struct request_queue *q = cdi->disk->queue;
	struct request *rq;
	struct scsi_request *req;
	struct bio *bio;
	unsigned int len;
	int nr, ret = 0;

	if (!q)
		return -ENXIO;

	if (!blk_queue_scsi_passthrough(q)) {
		WARN_ONCE(true,
			  "Attempt read CDDA info through a non-SCSI queue\n");
		return -EINVAL;
	}

	cdi->last_sense = 0;

	while (nframes) {
		nr = nframes;
		if (cdi->cdda_method == CDDA_BPC_SINGLE)
			nr = 1;
		if (nr * CD_FRAMESIZE_RAW > (queue_max_sectors(q) << 9))
			nr = (queue_max_sectors(q) << 9) / CD_FRAMESIZE_RAW;

		len = nr * CD_FRAMESIZE_RAW;

		rq = blk_get_request(q, REQ_OP_SCSI_IN, 0);
		if (IS_ERR(rq)) {
			ret = PTR_ERR(rq);
			break;
		}
		req = scsi_req(rq);

		ret = blk_rq_map_user(q, rq, NULL, ubuf, len, GFP_KERNEL);
		if (ret) {
			blk_put_request(rq);
			break;
		}

		req->cmd[0] = GPCMD_READ_CD;
		req->cmd[1] = 1 << 2;
		req->cmd[2] = (lba >> 24) & 0xff;
		req->cmd[3] = (lba >> 16) & 0xff;
		req->cmd[4] = (lba >>  8) & 0xff;
		req->cmd[5] = lba & 0xff;
		req->cmd[6] = (nr >> 16) & 0xff;
		req->cmd[7] = (nr >>  8) & 0xff;
		req->cmd[8] = nr & 0xff;
		req->cmd[9] = 0xf8;

		req->cmd_len = 12;
		rq->timeout = 60 * HZ;
		bio = rq->bio;

		blk_execute_rq(q, cdi->disk, rq, 0);
		if (scsi_req(rq)->result) {
			struct scsi_sense_hdr sshdr;

			ret = -EIO;
			scsi_normalize_sense(req->sense, req->sense_len,
					     &sshdr);
			cdi->last_sense = sshdr.sense_key;
		}

		if (blk_rq_unmap_user(bio))
			ret = -EFAULT;
		blk_put_request(rq);

		if (ret)
			break;

		nframes -= nr;
		lba += nr;
		ubuf += len;
	}

	return ret;
}