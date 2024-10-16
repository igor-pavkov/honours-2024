void update_stat_ex_create_time(struct stat_ex *dst,
                                struct timespec create_time)
{
	dst->st_ex_btime = create_time;
	dst->st_ex_calculated_birthtime = false;
}