static time_t gf_mktime_utc(struct tm *tm)
{
	return timegm64(tm);
}