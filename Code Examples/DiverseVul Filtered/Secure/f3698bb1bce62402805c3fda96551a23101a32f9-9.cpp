u64 gf_sys_clock_high_res()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	return (now.tv_sec)*1000000 + (now.tv_usec) - sys_start_time_hr;
}