int set_xattr_acl(const char *fname, int is_access_acl, const char *buf, size_t buf_len)
{
	const char *name = is_access_acl ? XACC_ACL_ATTR : XDEF_ACL_ATTR;
	if (sys_lsetxattr(fname, name, buf, buf_len) < 0) {
		rsyserr(FERROR_XFER, errno,
			"set_xattr_acl: lsetxattr(%s,\"%s\") failed",
			full_fname(fname), name);
		return -1;
	}
	return 0;
}