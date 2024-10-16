static ssize_t cgroup1_procs_write(struct kernfs_open_file *of,
				   char *buf, size_t nbytes, loff_t off)
{
	return __cgroup1_procs_write(of, buf, nbytes, off, true);
}