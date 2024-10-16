s32 gf_net_get_timezone()
{
#if defined(_WIN32_WCE)
	return 0;
#else
	//this has been commented due to some reports of broken implementation on some systems ...
	//		s32 val = timezone;
	//		return val;


	/*FIXME - avoid errors at midnight when estimating timezone this does not work !!*/
	s32 t_timezone;
	struct tm t_gmt, t_local;
	time_t t_time;
	t_time = time(NULL);
	t_gmt = *gmtime(&t_time);
	t_local = *localtime(&t_time);

	t_timezone = (t_gmt.tm_hour - t_local.tm_hour) * 3600 + (t_gmt.tm_min - t_local.tm_min) * 60;
	return t_timezone;
#endif

}