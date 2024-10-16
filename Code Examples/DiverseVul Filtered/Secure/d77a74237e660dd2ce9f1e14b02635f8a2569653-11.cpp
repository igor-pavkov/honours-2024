void update_stat_ex_mtime(struct stat_ex *dst,
				struct timespec write_ts)
{
	dst->st_ex_mtime = write_ts;

	/* We may have to recalculate btime. */
	if (dst->st_ex_calculated_birthtime) {
		dst->st_ex_btime = calc_create_time_stat_ex(dst);
	}
}