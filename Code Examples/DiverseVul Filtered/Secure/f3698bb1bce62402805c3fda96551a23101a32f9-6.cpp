void gf_sys_close()
{
	if (sys_init > 0) {
		sys_init --;
		if (sys_init) return;
		/*prevent any call*/
		last_update_time = 0xFFFFFFFF;

#if defined(WIN32) && !defined(_WIN32_WCE)
		timeEndPeriod(1);

		MyGetSystemTimes = NULL;
		MyGetProcessMemoryInfo = NULL;
		MyQuerySystemInfo = NULL;
		if (psapi_hinst) FreeLibrary(psapi_hinst);
		psapi_hinst = NULL;
#endif
	}
}