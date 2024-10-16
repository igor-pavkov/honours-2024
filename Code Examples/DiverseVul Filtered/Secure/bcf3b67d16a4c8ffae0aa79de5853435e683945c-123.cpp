static int megasas_mgmt_compat_ioctl_fw(struct file *file, unsigned long arg)
{
	struct compat_megasas_iocpacket __user *cioc =
	    (struct compat_megasas_iocpacket __user *)arg;
	struct megasas_iocpacket __user *ioc =
	    compat_alloc_user_space(sizeof(struct megasas_iocpacket));
	int i;
	int error = 0;
	compat_uptr_t ptr;
	u32 local_sense_off;
	u32 local_sense_len;
	u32 user_sense_off;

	if (clear_user(ioc, sizeof(*ioc)))
		return -EFAULT;

	if (copy_in_user(&ioc->host_no, &cioc->host_no, sizeof(u16)) ||
	    copy_in_user(&ioc->sgl_off, &cioc->sgl_off, sizeof(u32)) ||
	    copy_in_user(&ioc->sense_off, &cioc->sense_off, sizeof(u32)) ||
	    copy_in_user(&ioc->sense_len, &cioc->sense_len, sizeof(u32)) ||
	    copy_in_user(ioc->frame.raw, cioc->frame.raw, 128) ||
	    copy_in_user(&ioc->sge_count, &cioc->sge_count, sizeof(u32)))
		return -EFAULT;

	/*
	 * The sense_ptr is used in megasas_mgmt_fw_ioctl only when
	 * sense_len is not null, so prepare the 64bit value under
	 * the same condition.
	 */
	if (get_user(local_sense_off, &ioc->sense_off) ||
		get_user(local_sense_len, &ioc->sense_len) ||
		get_user(user_sense_off, &cioc->sense_off))
		return -EFAULT;

	if (local_sense_off != user_sense_off)
		return -EINVAL;

	if (local_sense_len) {
		void __user **sense_ioc_ptr =
			(void __user **)((u8 *)((unsigned long)&ioc->frame.raw) + local_sense_off);
		compat_uptr_t *sense_cioc_ptr =
			(compat_uptr_t *)(((unsigned long)&cioc->frame.raw) + user_sense_off);
		if (get_user(ptr, sense_cioc_ptr) ||
		    put_user(compat_ptr(ptr), sense_ioc_ptr))
			return -EFAULT;
	}

	for (i = 0; i < MAX_IOCTL_SGE; i++) {
		if (get_user(ptr, &cioc->sgl[i].iov_base) ||
		    put_user(compat_ptr(ptr), &ioc->sgl[i].iov_base) ||
		    copy_in_user(&ioc->sgl[i].iov_len,
				 &cioc->sgl[i].iov_len, sizeof(compat_size_t)))
			return -EFAULT;
	}

	error = megasas_mgmt_ioctl_fw(file, (unsigned long)ioc);

	if (copy_in_user(&cioc->frame.hdr.cmd_status,
			 &ioc->frame.hdr.cmd_status, sizeof(u8))) {
		printk(KERN_DEBUG "megasas: error copy_in_user cmd_status\n");
		return -EFAULT;
	}
	return error;
}