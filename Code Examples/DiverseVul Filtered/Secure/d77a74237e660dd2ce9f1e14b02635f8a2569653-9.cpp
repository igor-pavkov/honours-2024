int sys_fstat(int fd, SMB_STRUCT_STAT *sbuf, bool fake_dir_create_times)
{
	int ret;
	struct stat statbuf;
	ret = fstat(fd, &statbuf);
	if (ret == 0) {
		/* we always want directories to appear zero size */
		if (S_ISDIR(statbuf.st_mode)) {
			statbuf.st_size = 0;
		}
		init_stat_ex_from_stat(sbuf, &statbuf, fake_dir_create_times);
	}
	return ret;
}