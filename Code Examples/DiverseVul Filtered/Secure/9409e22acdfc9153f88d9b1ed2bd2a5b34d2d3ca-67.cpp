int kern_path(const char *name, unsigned int flags, struct path *path)
{
	return filename_lookup(AT_FDCWD, getname_kernel(name),
			       flags, path, NULL);
}