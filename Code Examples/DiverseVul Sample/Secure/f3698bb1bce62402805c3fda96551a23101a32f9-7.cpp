static u64 OS_GetSysClockHIGHRES_FULL()
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	now.QuadPart -= init_counter.QuadPart;
	return (u64) ((now.QuadPart * 1000000) / frequency.QuadPart);
}