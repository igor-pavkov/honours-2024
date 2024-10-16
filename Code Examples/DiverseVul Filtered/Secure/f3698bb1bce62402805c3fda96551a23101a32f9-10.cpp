u64 gf_net_get_ntp_ts()
{
	u64 res;
	u32 sec, frac;
	gf_net_get_ntp(&sec, &frac);
	res = sec;
	res<<= 32;
	res |= frac;
	return res;
}