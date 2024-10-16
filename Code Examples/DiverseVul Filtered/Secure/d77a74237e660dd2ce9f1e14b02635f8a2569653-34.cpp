static void make_create_timespec(const struct stat *pst, struct stat_ex *dst,
				 bool fake_dir_create_times)
{
	if (S_ISDIR(pst->st_mode) && fake_dir_create_times) {
		dst->st_ex_btime.tv_sec = 315493200L;          /* 1/1/1980 */
		dst->st_ex_btime.tv_nsec = 0;
	}

	dst->st_ex_calculated_birthtime = false;

#if defined(HAVE_STRUCT_STAT_ST_BIRTHTIMESPEC_TV_NSEC)
	dst->st_ex_btime = pst->st_birthtimespec;
#elif defined(HAVE_STRUCT_STAT_ST_BIRTHTIMENSEC)
	dst->st_ex_btime.tv_sec = pst->st_birthtime;
	dst->st_ex_btime.tv_nsec = pst->st_birthtimenspec;
#elif defined(HAVE_STRUCT_STAT_ST_BIRTHTIME)
	dst->st_ex_btime.tv_sec = pst->st_birthtime;
	dst->st_ex_btime.tv_nsec = 0;
#else
	dst->st_ex_btime = calc_create_time_stat(pst);
	dst->st_ex_calculated_birthtime = true;
#endif

	/* Deal with systems that don't initialize birthtime correctly.
	 * Pointed out by SATOH Fumiyasu <fumiyas@osstech.jp>.
	 */
	if (null_timespec(dst->st_ex_btime)) {
		dst->st_ex_btime = calc_create_time_stat(pst);
		dst->st_ex_calculated_birthtime = true;
	}
}