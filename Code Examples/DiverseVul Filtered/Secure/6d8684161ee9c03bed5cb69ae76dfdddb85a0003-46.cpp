int gettimeofday(struct timeval *tv, void *tz)
{
	FILETIME ft;
	long long hnsec;

	GetSystemTimeAsFileTime(&ft);
	hnsec = filetime_to_hnsec(&ft);
	tv->tv_sec = hnsec / 10000000;
	tv->tv_usec = (hnsec % 10000000) / 10;
	return 0;
}