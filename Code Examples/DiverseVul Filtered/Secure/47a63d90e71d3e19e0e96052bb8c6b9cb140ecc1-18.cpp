static char *get_xattr_data(const char *fname, const char *name, size_t *len_ptr,
			    int no_missing_error)
{
	size_t datum_len = sys_lgetxattr(fname, name, NULL, 0);
	size_t extra_len = *len_ptr;
	char *ptr;

	*len_ptr = datum_len;

	if (datum_len == (size_t)-1) {
		if (errno == ENOTSUP || no_missing_error)
			return NULL;
		rsyserr(FERROR_XFER, errno,
			"get_xattr_data: lgetxattr(%s,\"%s\",0) failed",
			full_fname(fname), name);
		return NULL;
	}

	if (!datum_len && !extra_len)
		extra_len = 1; /* request non-zero amount of memory */
	if (datum_len + extra_len < datum_len)
		overflow_exit("get_xattr_data");
	if (!(ptr = new_array(char, datum_len + extra_len)))
		out_of_memory("get_xattr_data");

	if (datum_len) {
		size_t len = sys_lgetxattr(fname, name, ptr, datum_len);
		if (len != datum_len) {
			if (len == (size_t)-1) {
				rsyserr(FERROR_XFER, errno,
				    "get_xattr_data: lgetxattr(%s,\"%s\",%ld) failed",
				    full_fname(fname), name, (long)datum_len);
			} else {
				rprintf(FERROR_XFER,
				    "get_xattr_data: lgetxattr(%s,\"%s\",%ld) returned %ld\n",
				    full_fname(fname), name,
				    (long)datum_len, (long)len);
			}
			free(ptr);
			return NULL;
		}
	}

	return ptr;
}