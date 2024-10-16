static bool is_dvbv3_delsys(u32 delsys)
{
	return (delsys == SYS_DVBT) || (delsys == SYS_DVBC_ANNEX_A) ||
	       (delsys == SYS_DVBS) || (delsys == SYS_ATSC);
}