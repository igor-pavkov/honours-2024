int set_stat_xattr(const char *fname, struct file_struct *file, mode_t new_mode)
{
	STRUCT_STAT fst, xst;
	dev_t rdev;
	mode_t mode, fmode;

	if (dry_run)
		return 0;

	if (read_only || list_only) {
		rsyserr(FERROR_XFER, EROFS, "failed to write xattr %s for %s",
			XSTAT_ATTR, full_fname(fname));
		return -1;
	}

	if (x_lstat(fname, &fst, &xst) < 0) {
		rsyserr(FERROR_XFER, errno, "failed to re-stat %s",
			full_fname(fname));
		return -1;
	}

	fst.st_mode &= (_S_IFMT | CHMOD_BITS);
	fmode = new_mode & (_S_IFMT | CHMOD_BITS);

	if (IS_DEVICE(fmode)) {
		uint32 *devp = F_RDEV_P(file);
		rdev = MAKEDEV(DEV_MAJOR(devp), DEV_MINOR(devp));
	} else
		rdev = 0;

	/* Dump the special permissions and enable full owner access. */
	mode = (fst.st_mode & _S_IFMT) | (fmode & ACCESSPERMS)
	     | (S_ISDIR(fst.st_mode) ? 0700 : 0600);
	if (fst.st_mode != mode)
		do_chmod(fname, mode);
	if (!IS_DEVICE(fst.st_mode))
		fst.st_rdev = 0; /* just in case */

	if (mode == fmode && fst.st_rdev == rdev
	 && fst.st_uid == F_OWNER(file) && fst.st_gid == F_GROUP(file)) {
		/* xst.st_mode will be 0 if there's no current stat xattr */
		if (xst.st_mode && sys_lremovexattr(fname, XSTAT_ATTR) < 0) {
			rsyserr(FERROR_XFER, errno,
				"delete of stat xattr failed for %s",
				full_fname(fname));
			return -1;
		}
		return 0;
	}

	if (xst.st_mode != fmode || xst.st_rdev != rdev
	 || xst.st_uid != F_OWNER(file) || xst.st_gid != F_GROUP(file)) {
		char buf[256];
		int len = snprintf(buf, sizeof buf, "%o %u,%u %u:%u",
			to_wire_mode(fmode),
			(int)major(rdev), (int)minor(rdev),
			F_OWNER(file), F_GROUP(file));
		if (sys_lsetxattr(fname, XSTAT_ATTR, buf, len) < 0) {
			if (errno == EPERM && S_ISLNK(fst.st_mode))
				return 0;
			rsyserr(FERROR_XFER, errno,
				"failed to write xattr %s for %s",
				XSTAT_ATTR, full_fname(fname));
			return -1;
		}
	}

	return 0;
}