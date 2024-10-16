int x_lstat(const char *fname, STRUCT_STAT *fst, STRUCT_STAT *xst)
{
	int ret = do_lstat(fname, fst);
	if ((ret < 0 || get_stat_xattr(fname, -1, fst, xst) < 0) && xst)
		xst->st_mode = 0;
	return ret;
}