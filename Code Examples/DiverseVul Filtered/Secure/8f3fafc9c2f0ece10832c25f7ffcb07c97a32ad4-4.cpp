static noinline int mmc_ioctl_cdrom_read_data(struct cdrom_device_info *cdi,
					      void __user *arg,
					      struct packet_command *cgc,
					      int cmd)
{
	struct scsi_sense_hdr sshdr;
	struct cdrom_msf msf;
	int blocksize = 0, format = 0, lba;
	int ret;

	switch (cmd) {
	case CDROMREADRAW:
		blocksize = CD_FRAMESIZE_RAW;
		break;
	case CDROMREADMODE1:
		blocksize = CD_FRAMESIZE;
		format = 2;
		break;
	case CDROMREADMODE2:
		blocksize = CD_FRAMESIZE_RAW0;
		break;
	}
	if (copy_from_user(&msf, (struct cdrom_msf __user *)arg, sizeof(msf)))
		return -EFAULT;
	lba = msf_to_lba(msf.cdmsf_min0, msf.cdmsf_sec0, msf.cdmsf_frame0);
	/* FIXME: we need upper bound checking, too!! */
	if (lba < 0)
		return -EINVAL;

	cgc->buffer = kzalloc(blocksize, GFP_KERNEL);
	if (cgc->buffer == NULL)
		return -ENOMEM;

	memset(&sshdr, 0, sizeof(sshdr));
	cgc->sshdr = &sshdr;
	cgc->data_direction = CGC_DATA_READ;
	ret = cdrom_read_block(cdi, cgc, lba, 1, format, blocksize);
	if (ret && sshdr.sense_key == 0x05 &&
	    sshdr.asc == 0x20 &&
	    sshdr.ascq == 0x00) {
		/*
		 * SCSI-II devices are not required to support
		 * READ_CD, so let's try switching block size
		 */
		/* FIXME: switch back again... */
		ret = cdrom_switch_blocksize(cdi, blocksize);
		if (ret)
			goto out;
		cgc->sshdr = NULL;
		ret = cdrom_read_cd(cdi, cgc, lba, blocksize, 1);
		ret |= cdrom_switch_blocksize(cdi, blocksize);
	}
	if (!ret && copy_to_user(arg, cgc->buffer, blocksize))
		ret = -EFAULT;
out:
	kfree(cgc->buffer);
	return ret;
}