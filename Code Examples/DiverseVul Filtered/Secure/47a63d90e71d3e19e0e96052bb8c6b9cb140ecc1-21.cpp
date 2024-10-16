int copy_xattrs(const char *source, const char *dest)
{
	ssize_t list_len, name_len;
	size_t datum_len;
	char *name, *ptr;
#ifdef HAVE_LINUX_XATTRS
	int user_only = am_sender ? 0 : am_root <= 0;
#endif

	/* This puts the name list into the "namebuf" buffer. */
	if ((list_len = get_xattr_names(source)) < 0)
		return -1;

	for (name = namebuf; list_len > 0; name += name_len) {
		name_len = strlen(name) + 1;
		list_len -= name_len;

		if (saw_xattr_filter) {
			if (name_is_excluded(name, NAME_IS_XATTR, ALL_FILTERS))
				continue;
		}
#ifdef HAVE_LINUX_XATTRS
		/* Choose between ignoring the system namespace or (non-root) ignoring any non-user namespace. */
		else if (user_only ? !HAS_PREFIX(name, USER_PREFIX) : HAS_PREFIX(name, SYSTEM_PREFIX))
			continue;
#endif

		datum_len = 0;
		if (!(ptr = get_xattr_data(source, name, &datum_len, 0)))
			return -1;
		if (sys_lsetxattr(dest, name, ptr, datum_len) < 0) {
			int save_errno = errno ? errno : EINVAL;
			rsyserr(FERROR_XFER, errno,
				"copy_xattrs: lsetxattr(%s,\"%s\") failed",
				full_fname(dest), name);
			errno = save_errno;
			return -1;
		}
		free(ptr);
	}

	return 0;
}