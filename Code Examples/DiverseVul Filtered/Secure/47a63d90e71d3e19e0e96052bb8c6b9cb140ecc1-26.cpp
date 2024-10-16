char *get_xattr_acl(const char *fname, int is_access_acl, size_t *len_p)
{
	const char *name = is_access_acl ? XACC_ACL_ATTR : XDEF_ACL_ATTR;
	*len_p = 0; /* no extra data alloc needed from get_xattr_data() */
	return get_xattr_data(fname, name, len_p, 1);
}