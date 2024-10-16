static time_t gf_mktime_utc(struct tm *tm)
{
	static const u32 days_per_month[2][12] = {
		{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
		{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
	};
	time_t time=0;
	int i;

	for (i=70; i<tm->tm_year; i++) {
		time += leap_year(i) ? 366 : 365;
	}

	for (i=0; i<tm->tm_mon; ++i) {
		time += days_per_month[leap_year(tm->tm_year)][i];
	}
	time += tm->tm_mday - 1;
	time *= 24;
	time += tm->tm_hour;
	time *= 60;
	time += tm->tm_min;
	time *= 60;
	time += tm->tm_sec;
	return time;
}