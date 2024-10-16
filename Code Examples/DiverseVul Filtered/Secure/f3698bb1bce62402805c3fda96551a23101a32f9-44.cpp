void gf_sys_init(GF_MemTrackerType mem_tracker_type)
{
	if (!sys_init) {
#if defined (WIN32)
#if defined(_WIN32_WCE)
		MEMORYSTATUS ms;
#else
		SYSTEM_INFO sysinfo;
#endif
#endif

		if (mem_tracker_type!=GF_MemTrackerNone) {
#ifdef GPAC_MEMORY_TRACKING
            gf_mem_enable_tracker( (mem_tracker_type==GF_MemTrackerBackTrace) ? GF_TRUE : GF_FALSE);
#endif
		}
#ifndef GPAC_DISABLE_LOG
		/*by default log subsystem is initialized to error on all tools, and info on console to debug scripts*/
		gf_log_set_tool_level(GF_LOG_ALL, GF_LOG_ERROR);
		gf_log_set_tool_level(GF_LOG_CONSOLE, GF_LOG_INFO);
#endif


#if defined(__sh__)
		/* Round all denormalized floatting point number to 0.0 */
		sh4_change_fpscr(0,SH4_FPSCR_DN) ;
#endif

#if defined(WIN32)
		frequency.QuadPart = 0;
		/*clock setup*/
		if (QueryPerformanceFrequency(&frequency)) {
			QueryPerformanceCounter(&init_counter);
			OS_GetSysClock = OS_GetSysClockHIGHRES;
			OS_GetSysClockHR = OS_GetSysClockHIGHRES_FULL;
			GF_LOG(GF_LOG_INFO, GF_LOG_CORE, ("[core] using WIN32 performance timer\n"));
		} else {
			OS_GetSysClock = OS_GetSysClockNORMAL;
			OS_GetSysClockHR = OS_GetSysClockNORMAL_FULL;
			GF_LOG(GF_LOG_INFO, GF_LOG_CORE, ("[core] using WIN32 regular timer\n"));
		}

#ifndef _WIN32_WCE
		timeBeginPeriod(1);
#endif

		GF_LOG(GF_LOG_INFO, GF_LOG_CORE, ("[core] checking for run-time info tools"));
#if defined(_WIN32_WCE)
		last_total_k_u_time = last_process_k_u_time = 0;
		last_update_time = 0;
		memset(&the_rti, 0, sizeof(GF_SystemRTInfo));
		the_rti.pid = GetCurrentProcessId();
		the_rti.nb_cores = 1;
		GlobalMemoryStatus(&ms);
		mem_usage_at_startup = ms.dwAvailPhys;
#else
		/*cpu usage tools are buried in win32 dlls...*/
		MyGetSystemTimes = (NTGetSystemTimes) GetProcAddress(GetModuleHandle("kernel32.dll"), "GetSystemTimes");
		if (!MyGetSystemTimes) {
			MyQuerySystemInfo = (NTQuerySystemInfo) GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQuerySystemInformation");
			if (MyQuerySystemInfo) {
				GF_LOG(GF_LOG_INFO, GF_LOG_CORE, (" - CPU: QuerySystemInformation"));
			}
		} else {
			GF_LOG(GF_LOG_INFO, GF_LOG_CORE, (" - CPU: GetSystemsTimes"));
		}
		psapi_hinst = LoadLibrary("psapi.dll");
		MyGetProcessMemoryInfo = (NTGetProcessMemoryInfo) GetProcAddress(psapi_hinst, "GetProcessMemoryInfo");
		if (MyGetProcessMemoryInfo) {
			GF_LOG(GF_LOG_INFO, GF_LOG_CORE, (" - memory: GetProcessMemoryInfo"));
		}
		last_process_k_u_time = last_proc_idle_time = last_proc_k_u_time = 0;
		last_update_time = 0;
		memset(&the_rti, 0, sizeof(GF_SystemRTInfo));
		the_rti.pid = GetCurrentProcessId();

		GetSystemInfo( &sysinfo );
		the_rti.nb_cores = sysinfo.dwNumberOfProcessors;
#endif
		GF_LOG(GF_LOG_INFO, GF_LOG_CORE, ("\n"));

#else
		/*linux threads and OSX...*/
		last_process_k_u_time = 0;
		last_cpu_u_k_time = last_cpu_idle_time = 0;
		last_update_time = 0;
		memset(&the_rti, 0, sizeof(GF_SystemRTInfo));
		the_rti.pid = getpid();
		the_rti.nb_cores = (u32) sysconf( _SC_NPROCESSORS_ONLN );
		sys_start_time = gf_sys_clock();
		sys_start_time_hr = gf_sys_clock_high_res();
#endif
		GF_LOG(GF_LOG_INFO, GF_LOG_CORE, ("[core] process id %d\n", the_rti.pid));

#ifndef _WIN32_WCE
		setlocale( LC_NUMERIC, "C" );
#endif
	}
	sys_init += 1;


	/*init RTI stats*/
	if (!memory_at_gpac_startup) {
		GF_SystemRTInfo rti;
		if (gf_sys_get_rti(500, &rti, GF_RTI_SYSTEM_MEMORY_ONLY)) {
			memory_at_gpac_startup = rti.physical_memory_avail;
			GF_LOG(GF_LOG_INFO, GF_LOG_CORE, ("[core] System init OK - process id %d - %d MB physical RAM - %d cores\n", rti.pid, (u32) (rti.physical_memory/1024/1024), rti.nb_cores));
		} else {
			memory_at_gpac_startup = 0;
		}
	}
}