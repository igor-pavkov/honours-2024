static int fd_compat_ioctl(struct block_device *bdev, fmode_t mode, unsigned int cmd,
		    unsigned long param)
{
	int drive = (long)bdev->bd_disk->private_data;
	switch (cmd) {
	case CDROMEJECT: /* CD-ROM eject */
	case 0x6470:	 /* SunOS floppy eject */

	case FDMSGON:
	case FDMSGOFF:
	case FDSETEMSGTRESH:
	case FDFLUSH:
	case FDWERRORCLR:
	case FDEJECT:
	case FDCLRPRM:
	case FDFMTBEG:
	case FDRESET:
	case FDTWADDLE:
		return fd_ioctl(bdev, mode, cmd, param);
	case FDSETMAXERRS:
	case FDGETMAXERRS:
	case FDGETDRVTYP:
	case FDFMTEND:
	case FDFMTTRK:
	case FDRAWCMD:
		return fd_ioctl(bdev, mode, cmd,
				(unsigned long)compat_ptr(param));
	case FDSETPRM32:
	case FDDEFPRM32:
		return compat_set_geometry(bdev, mode, cmd, compat_ptr(param));
	case FDGETPRM32:
		return compat_get_prm(drive, compat_ptr(param));
	case FDSETDRVPRM32:
		return compat_setdrvprm(drive, compat_ptr(param));
	case FDGETDRVPRM32:
		return compat_getdrvprm(drive, compat_ptr(param));
	case FDPOLLDRVSTAT32:
		return compat_getdrvstat(drive, true, compat_ptr(param));
	case FDGETDRVSTAT32:
		return compat_getdrvstat(drive, false, compat_ptr(param));
	case FDGETFDCSTAT32:
		return compat_getfdcstat(drive, compat_ptr(param));
	case FDWERRORGET32:
		return compat_werrorget(drive, compat_ptr(param));
	}
	return -EINVAL;
}