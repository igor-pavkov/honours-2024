static int md_compat_ioctl(struct block_device *bdev, fmode_t mode,
		    unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case HOT_REMOVE_DISK:
	case HOT_ADD_DISK:
	case SET_DISK_FAULTY:
	case SET_BITMAP_FILE:
		/* These take in integer arg, do not convert */
		break;
	default:
		arg = (unsigned long)compat_ptr(arg);
		break;
	}

	return md_ioctl(bdev, mode, cmd, arg);
}