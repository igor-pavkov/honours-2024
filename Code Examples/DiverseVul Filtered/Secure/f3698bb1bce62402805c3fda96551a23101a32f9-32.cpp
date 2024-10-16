static s32 gettimeofday(struct timeval *tp, void *tz)
{
	struct _timeb timebuffer;

	_ftime( &timebuffer );
	tp->tv_sec  = (long) (timebuffer.time);
	tp->tv_usec = timebuffer.millitm * 1000;
	return 0;
}