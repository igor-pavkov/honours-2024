void gf_sleep(u32 ms)
{
#ifdef WIN32
	Sleep(ms);
#else
	s32 sel_err;
	struct timeval tv;

#ifndef SLEEP_ABS_SELECT
	u32 prev, now, elapsed;
#endif

#ifdef SLEEP_ABS_SELECT
	tv.tv_sec = ms/1000;
	tv.tv_usec = (ms%1000)*1000;
#else
	prev = gf_sys_clock();
#endif

	do {
		errno = 0;

#ifndef SLEEP_ABS_SELECT
		now = gf_sys_clock();
		elapsed = (now - prev);
		if ( elapsed >= ms ) {
			break;
		}
		prev = now;
		ms -= elapsed;
		tv.tv_sec = ms/1000;
		tv.tv_usec = (ms%1000)*1000;
#endif

		sel_err = select(0, NULL, NULL, NULL, &tv);
	} while ( sel_err && (errno == EINTR) );
#endif
}