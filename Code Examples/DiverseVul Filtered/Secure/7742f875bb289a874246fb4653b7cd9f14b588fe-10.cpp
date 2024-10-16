bool format_time(time_t time, char *dest, size_t max_size)
{
	tm tm;
	localtime_r(&time, &tm);
	return strftime(dest, max_size,
		 "%Y-%m-%d %H:%M:%S", &tm) != 0;
}