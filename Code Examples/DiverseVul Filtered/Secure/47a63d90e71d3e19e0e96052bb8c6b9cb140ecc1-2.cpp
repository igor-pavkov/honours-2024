int x_fstat(int fd, STRUCT_STAT *fst, STRUCT_STAT *xst)
{
	int ret = do_fstat(fd, fst);
	if ((ret < 0 || get_stat_xattr(NULL, fd, fst, xst) < 0) && xst)
		xst->st_mode = 0;
	return ret;
}