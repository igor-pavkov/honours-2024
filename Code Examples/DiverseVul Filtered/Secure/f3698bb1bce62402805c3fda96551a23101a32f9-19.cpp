void gf_utc_time_since_1970(u32 *sec, u32 *msec)
{
#if defined (WIN32) && !defined(_WIN32_WCE)
	struct _timeb	tb;
	_ftime( &tb );
	*sec = (u32) tb.time;
	*msec = tb.millitm;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	*sec = (u32) tv.tv_sec;
	*msec = tv.tv_usec/1000;
#endif
}