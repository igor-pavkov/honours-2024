int open_tmpfile(char *fnametmp, const char *fname, struct file_struct *file)
{
	int fd;
	mode_t added_perms;

	if (!get_tmpname(fnametmp, fname, False))
		return -1;

	if (am_root < 0) {
		/* For --fake-super, the file must be useable by the copying
		 * user, just like it would be for root. */
		added_perms = S_IRUSR|S_IWUSR;
	} else {
		/* For a normal copy, we need to be able to tweak things like xattrs. */
		added_perms = S_IWUSR;
	}

	/* We initially set the perms without the setuid/setgid bits or group
	 * access to ensure that there is no race condition.  They will be
	 * correctly updated after the right owner and group info is set.
	 * (Thanks to snabb@epipe.fi for pointing this out.) */
	fd = do_mkstemp(fnametmp, (file->mode|added_perms) & INITACCESSPERMS);

#if 0
	/* In most cases parent directories will already exist because their
	 * information should have been previously transferred, but that may
	 * not be the case with -R */
	if (fd == -1 && relative_paths && errno == ENOENT
	 && make_path(fnametmp, MKP_SKIP_SLASH | MKP_DROP_NAME) == 0) {
		/* Get back to name with XXXXXX in it. */
		get_tmpname(fnametmp, fname, False);
		fd = do_mkstemp(fnametmp, (file->mode|added_perms) & INITACCESSPERMS);
	}
#endif

	if (fd == -1) {
		rsyserr(FERROR_XFER, errno, "mkstemp %s failed",
			full_fname(fnametmp));
		return -1;
	}

	return fd;
}