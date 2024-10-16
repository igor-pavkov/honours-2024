Bool gf_sys_get_rti_os(u32 refresh_time_ms, GF_SystemRTInfo *rti, u32 flags)
{
	size_t length;
	u32 entry_time, i, percent;
	int mib[6];
	u64 result;
	int pagesize;
	u64 process_u_k_time;
	double utime, stime;
	vm_statistics_data_t vmstat;
	task_t task;
	kern_return_t error;
	thread_array_t thread_table;
	unsigned table_size;
	thread_basic_info_t thi;
	thread_basic_info_data_t thi_data;
	struct task_basic_info ti;
	mach_msg_type_number_t count = HOST_VM_INFO_COUNT, size = sizeof(ti);

	entry_time = gf_sys_clock();
	if (last_update_time && (entry_time - last_update_time < refresh_time_ms)) {
		memcpy(rti, &the_rti, sizeof(GF_SystemRTInfo));
		return 0;
	}

	mib[0] = CTL_HW;
	mib[1] = HW_PAGESIZE;
	length = sizeof(pagesize);
	if (sysctl(mib, 2, &pagesize, &length, NULL, 0) < 0) {
		return 0;
	}

	if (host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vmstat, &count) != KERN_SUCCESS) {
		return 0;
	}

	the_rti.physical_memory = (vmstat.wire_count + vmstat.active_count + vmstat.inactive_count + vmstat.free_count)* pagesize;
	the_rti.physical_memory_avail = vmstat.free_count * pagesize;

	if (!total_physical_memory) {
		mib[0] = CTL_HW;
		mib[1] = HW_MEMSIZE;
		length = sizeof(u64);
		if (sysctl(mib, 2, &result, &length, NULL, 0) >= 0) {
			total_physical_memory = result;
		}
	}
	the_rti.physical_memory = total_physical_memory;

	error = task_for_pid(mach_task_self(), the_rti.pid, &task);
	if (error) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CORE, ("[RTI] Cannot get process task for PID %d: error %d\n", the_rti.pid, error));
		return 0;
	}

	error = task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&ti, &size);
	if (error) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CORE, ("[RTI] Cannot get process task info (PID %d): error %d\n", the_rti.pid, error));
		return 0;
	}

	percent = 0;
	utime = ti.user_time.seconds + ti.user_time.microseconds * 1e-6;
	stime = ti.system_time.seconds + ti.system_time.microseconds * 1e-6;
	error = task_threads(task, &thread_table, &table_size);
	if (error != KERN_SUCCESS) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CORE, ("[RTI] Cannot get threads task for PID %d: error %d\n", the_rti.pid, error));
		return 0;
	}
	thi = &thi_data;
	for (i = 0; i != table_size; ++i) {
		count = THREAD_BASIC_INFO_COUNT;
		error = thread_info(thread_table[i], THREAD_BASIC_INFO, (thread_info_t)thi, &count);
		if (error != KERN_SUCCESS) {
			mach_error("[RTI] Unexpected thread_info() call return", error);
			GF_LOG(GF_LOG_WARNING, GF_LOG_CORE, ("[RTI] Unexpected thread info for PID %d\n", the_rti.pid));
			break;
		}
		if ((thi->flags & TH_FLAGS_IDLE) == 0) {
			utime += thi->user_time.seconds + thi->user_time.microseconds * 1e-6;
			stime += thi->system_time.seconds + thi->system_time.microseconds * 1e-6;
			percent +=  (u32) (100 * (double)thi->cpu_usage / TH_USAGE_SCALE);
		}
	}
	vm_deallocate(mach_task_self(), (vm_offset_t)thread_table, table_size * sizeof(thread_array_t));
	mach_port_deallocate(mach_task_self(), task);

	process_u_k_time = utime + stime;

	the_rti.sampling_instant = last_update_time;

	if (last_update_time) {
		the_rti.sampling_period_duration = (entry_time - last_update_time);
		the_rti.process_cpu_time_diff = (process_u_k_time - last_process_k_u_time) * 10;

		the_rti.total_cpu_time_diff = the_rti.sampling_period_duration;
		/*TODO*/
		the_rti.cpu_idle_time = 0;
		the_rti.total_cpu_usage = 0;
		if (!the_rti.process_cpu_time_diff) the_rti.process_cpu_time_diff = the_rti.total_cpu_time_diff;

		the_rti.process_cpu_usage = percent;
	} else {
		mem_at_startup = the_rti.physical_memory_avail;
	}
	the_rti.process_memory = mem_at_startup - the_rti.physical_memory_avail;

#ifdef GPAC_MEMORY_TRACKING
	the_rti.gpac_memory = gpac_allocated_memory;
#endif

	last_process_k_u_time = process_u_k_time;
	last_cpu_idle_time = 0;
	last_update_time = entry_time;
	memcpy(rti, &the_rti, sizeof(GF_SystemRTInfo));
	return 1;
}