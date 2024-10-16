static int selinux_ismaclabel(const char *name)
{
	return (strcmp(name, XATTR_SELINUX_SUFFIX) == 0);
}