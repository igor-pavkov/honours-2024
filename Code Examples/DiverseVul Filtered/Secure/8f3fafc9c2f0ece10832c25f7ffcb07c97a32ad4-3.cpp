static int cdrom_read_cdda_old(struct cdrom_device_info *cdi, __u8 __user *ubuf,
			       int lba, int nframes)
{
	struct packet_command cgc;
	int ret = 0;
	int nr;

	cdi->last_sense = 0;

	memset(&cgc, 0, sizeof(cgc));

	/*
	 * start with will ra.nframes size, back down if alloc fails
	 */
	nr = nframes;
	do {
		cgc.buffer = kmalloc_array(nr, CD_FRAMESIZE_RAW, GFP_KERNEL);
		if (cgc.buffer)
			break;

		nr >>= 1;
	} while (nr);

	if (!nr)
		return -ENOMEM;

	cgc.data_direction = CGC_DATA_READ;
	while (nframes > 0) {
		if (nr > nframes)
			nr = nframes;

		ret = cdrom_read_block(cdi, &cgc, lba, nr, 1, CD_FRAMESIZE_RAW);
		if (ret)
			break;
		if (copy_to_user(ubuf, cgc.buffer, CD_FRAMESIZE_RAW * nr)) {
			ret = -EFAULT;
			break;
		}
		ubuf += CD_FRAMESIZE_RAW * nr;
		nframes -= nr;
		lba += nr;
	}
	kfree(cgc.buffer);
	return ret;
}