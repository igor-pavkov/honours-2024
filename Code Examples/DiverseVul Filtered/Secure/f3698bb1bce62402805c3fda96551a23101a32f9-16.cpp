Bool gf_sys_get_rti(u32 refresh_time_ms, GF_SystemRTInfo *rti, u32 flags)
{
	Bool res = gf_sys_get_rti_os(refresh_time_ms, rti, flags);
	if (res) {
		if (!rti->process_memory) rti->process_memory = memory_at_gpac_startup - rti->physical_memory_avail;
		if (!rti->gpac_memory) rti->gpac_memory = memory_at_gpac_startup - rti->physical_memory_avail;
	}
	return res;
}