int del_def_xattr_acl(const char *fname)
{
	return sys_lremovexattr(fname, XDEF_ACL_ATTR);
}