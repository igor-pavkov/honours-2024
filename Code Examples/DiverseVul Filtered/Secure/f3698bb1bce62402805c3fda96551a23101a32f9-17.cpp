u64 gf_net_get_utc()
{
	u64 current_time;
	Double msec;
	u32 sec, frac;

	gf_net_get_ntp(&sec, &frac);
	current_time = sec - GF_NTP_SEC_1900_TO_1970;
	current_time *= 1000;
	msec = frac*1000.0;
	msec /= 0xFFFFFFFF;
	current_time += (u64) msec;
	return current_time;
}