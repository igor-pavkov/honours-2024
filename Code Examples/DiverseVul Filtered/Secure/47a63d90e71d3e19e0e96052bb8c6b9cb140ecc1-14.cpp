static ssize_t get_xattr_names(const char *fname)
{
	ssize_t list_len;
	int64 arg;

	if (!namebuf) {
		namebuf_len = 1024;
		namebuf = new_array(char, namebuf_len);
		if (!namebuf)
			out_of_memory("get_xattr_names");
	}

	while (1) {
		/* The length returned includes all the '\0' terminators. */
		list_len = sys_llistxattr(fname, namebuf, namebuf_len);
		if (list_len >= 0) {
			if ((size_t)list_len <= namebuf_len)
				break;
		} else if (errno == ENOTSUP)
			return 0;
		else if (errno != ERANGE) {
			arg = namebuf_len;
		  got_error:
			rsyserr(FERROR_XFER, errno,
				"get_xattr_names: llistxattr(%s,%s) failed",
				full_fname(fname), big_num(arg));
			return -1;
		}
		list_len = sys_llistxattr(fname, NULL, 0);
		if (list_len < 0) {
			arg = 0;
			goto got_error;
		}
		if (namebuf_len)
			free(namebuf);
		namebuf_len = list_len + 1024;
		namebuf = new_array(char, namebuf_len);
		if (!namebuf)
			out_of_memory("get_xattr_names");
	}

	return list_len;
}