static int add_mount(const char *source, const char *mnt, const char *type,
		     const char *opts)
{
	return fuse_mnt_add_mount(progname, source, mnt, type, opts);
}