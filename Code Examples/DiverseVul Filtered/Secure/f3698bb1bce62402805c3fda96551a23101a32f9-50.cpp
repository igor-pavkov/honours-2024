static u32 OS_GetSysClockNORMAL()
{
#ifdef _WIN32_WCE
	return GetTickCount();
#else
	return timeGetTime();
#endif
}