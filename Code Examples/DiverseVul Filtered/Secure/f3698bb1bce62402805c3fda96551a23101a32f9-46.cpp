Bool gf_sys_get_rti_os(u32 refresh_time_ms, GF_SystemRTInfo *rti, u32 flags)
{
	u32 entry_time;
	u64 process_u_k_time;
	u32 u_k_time, idle_time;
#if 0
	char szProc[100];
#endif
	char line[2048];
	FILE *f;

	assert(sys_init);

	entry_time = gf_sys_clock();
	if (last_update_time && (entry_time - last_update_time < refresh_time_ms)) {
		memcpy(rti, &the_rti, sizeof(GF_SystemRTInfo));
		return 0;
	}

	u_k_time = idle_time = 0;
	f = gf_fopen("/proc/stat", "r");
	if (f) {
		u32 k_time, nice_time, u_time;
		if (fgets(line, 128, f) != NULL) {
			if (sscanf(line, "cpu  %u %u %u %u\n", &u_time, &k_time, &nice_time, &idle_time) == 4) {
				u_k_time = u_time + k_time + nice_time;
			}
		}
		gf_fclose(f);
	}

	process_u_k_time = 0;
	the_rti.process_memory = 0;

	/*FIXME? under LinuxThreads this will only fetch stats for the calling thread, we would have to enumerate /proc to get
	the complete CPU usage of all therads of the process...*/
#if 0
	sprintf(szProc, "/proc/%d/stat", the_rti.pid);
	f = gf_fopen(szProc, "r");
	if (f) {
		fflush(f);
		if (fgets(line, 2048, f) != NULL) {
			char state;
			char *start;
			long cutime, cstime, priority, nice, itrealvalue, rss;
			int exit_signal, processor;
			unsigned long flags, minflt, cminflt, majflt, cmajflt, utime, stime,starttime, vsize, rlim, startcode, endcode, startstack, kstkesp, kstkeip, signal, blocked, sigignore, sigcatch, wchan, nswap, cnswap, rem;
			int ppid, pgrp ,session, tty_nr, tty_pgrp, res;
			start = strchr(line, ')');
			if (start) start += 2;
			else {
				start = strchr(line, ' ');
				start++;
			}
			res = sscanf(start,"%c %d %d %d %d %d %lu %lu %lu %lu \
%lu %lu %lu %ld %ld %ld %ld %ld %ld %lu \
%lu %ld %lu %lu %lu %lu %lu %lu %lu %lu \
%lu %lu %lu %lu %lu %d %d",
			             &state, &ppid, &pgrp, &session, &tty_nr, &tty_pgrp, &flags, &minflt, &cminflt, &majflt,
			             &cmajflt, &utime, &stime, &cutime, &cstime, &priority, &nice, &itrealvalue, &rem, &starttime,
			             &vsize, &rss, &rlim, &startcode, &endcode, &startstack, &kstkesp, &kstkeip, &signal, &blocked,
			             &sigignore, &sigcatch, &wchan, &nswap, &cnswap, &exit_signal, &processor);

			if (res) process_u_k_time = (u64) (cutime + cstime);
			else {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CORE, ("[RTI] PROC %s parse error\n", szProc));
			}
		} else {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CORE, ("[RTI] error reading pid/stat\n\n", szProc));
		}
		gf_fclose(f);
	} else {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CORE, ("[RTI] cannot open %s\n", szProc));
	}
	sprintf(szProc, "/proc/%d/status", the_rti.pid);
	f = gf_fopen(szProc, "r");
	if (f) {
		while (fgets(line, 1024, f) != NULL) {
			if (!strnicmp(line, "VmSize:", 7)) {
				sscanf(line, "VmSize: %"LLD" kB",  &the_rti.process_memory);
				the_rti.process_memory *= 1024;
			}
		}
		gf_fclose(f);
	} else {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CORE, ("[RTI] cannot open %s\n", szProc));
	}
#endif


#ifndef GPAC_IPHONE
	the_rti.physical_memory = the_rti.physical_memory_avail = 0;
	f = gf_fopen("/proc/meminfo", "r");
	if (f) {
		while (fgets(line, 1024, f) != NULL) {
			if (!strnicmp(line, "MemTotal:", 9)) {
				sscanf(line, "MemTotal: "LLU" kB",  &the_rti.physical_memory);
				the_rti.physical_memory *= 1024;
			} else if (!strnicmp(line, "MemFree:", 8)) {
				sscanf(line, "MemFree: "LLU" kB",  &the_rti.physical_memory_avail);
				the_rti.physical_memory_avail *= 1024;
				break;
			}
		}
		gf_fclose(f);
	} else {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CORE, ("[RTI] cannot open /proc/meminfo\n"));
	}
#endif


	the_rti.sampling_instant = last_update_time;

	if (last_update_time) {
		the_rti.sampling_period_duration = (entry_time - last_update_time);
		the_rti.process_cpu_time_diff = (u32) (process_u_k_time - last_process_k_u_time) * 10;

		/*oops, we have no choice but to assume 100% cpu usage during this period*/
		if (!u_k_time) {
			the_rti.total_cpu_time_diff = the_rti.sampling_period_duration;
			u_k_time = (u32) (last_cpu_u_k_time + the_rti.sampling_period_duration);
			the_rti.cpu_idle_time = 0;
			the_rti.total_cpu_usage = 100;
			if (!the_rti.process_cpu_time_diff) the_rti.process_cpu_time_diff = the_rti.total_cpu_time_diff;
			the_rti.process_cpu_usage = (u32) ( 100 *  the_rti.process_cpu_time_diff / the_rti.sampling_period_duration);
		} else {
			u64 samp_sys_time;
			/*move to ms (/proc/stat gives times in 100 ms unit*/
			the_rti.total_cpu_time_diff = (u32) (u_k_time - last_cpu_u_k_time)*10;

			/*we're not that accurate....*/
			if (the_rti.total_cpu_time_diff > the_rti.sampling_period_duration)
				the_rti.sampling_period_duration = the_rti.total_cpu_time_diff;


			if (!idle_time) idle_time = (the_rti.sampling_period_duration - the_rti.total_cpu_time_diff)/10;
			samp_sys_time = u_k_time - last_cpu_u_k_time;
			the_rti.cpu_idle_time = (u32) (idle_time - last_cpu_idle_time);
			the_rti.total_cpu_usage = (u32) ( 100 * samp_sys_time / (the_rti.cpu_idle_time + samp_sys_time ) );
			/*move to ms (/proc/stat gives times in 100 ms unit*/
			the_rti.cpu_idle_time *= 10;
			if (!the_rti.process_cpu_time_diff) the_rti.process_cpu_time_diff = the_rti.total_cpu_time_diff;
			the_rti.process_cpu_usage = (u32) ( 100 *  the_rti.process_cpu_time_diff / (the_rti.cpu_idle_time + 10*samp_sys_time ) );
		}
	} else {
		mem_at_startup = the_rti.physical_memory_avail;
	}
	the_rti.process_memory = mem_at_startup - the_rti.physical_memory_avail;
#ifdef GPAC_MEMORY_TRACKING
	the_rti.gpac_memory = gpac_allocated_memory;
#endif

	last_process_k_u_time = process_u_k_time;
	last_cpu_idle_time = idle_time;
	last_cpu_u_k_time = u_k_time;
	last_update_time = entry_time;
	memcpy(rti, &the_rti, sizeof(GF_SystemRTInfo));
	return 1;
}