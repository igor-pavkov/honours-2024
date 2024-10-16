static u32 OS_GetSysClockHIGHRES()
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	now.QuadPart -= init_counter.QuadPart;
	return (u32) ((now.QuadPart * 1000) / frequency.QuadPart);
}