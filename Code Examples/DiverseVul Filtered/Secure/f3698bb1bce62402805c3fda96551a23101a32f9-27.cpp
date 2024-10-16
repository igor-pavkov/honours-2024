u32 gf_sys_clock()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	return (u32) ( ( (now.tv_sec)*1000 + (now.tv_usec) / 1000) - sys_start_time );
}