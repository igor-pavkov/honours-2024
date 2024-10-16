static u64 OS_GetSysClockNORMAL_FULL()
{
	u64 res = OS_GetSysClockNORMAL();
	return res*1000;
}