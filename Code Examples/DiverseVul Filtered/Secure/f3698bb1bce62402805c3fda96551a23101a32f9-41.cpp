s32 gf_net_get_ntp_diff_ms(u64 ntp)
{
	u32 remote_s, remote_f, local_s, local_f;
	s64 local, remote;

	remote_s = (ntp >> 32);
	remote_f = (u32) (ntp & 0xFFFFFFFFULL);
	gf_net_get_ntp(&local_s, &local_f);

	local = local_s;
	local *= 1000;
	local += ((u64) local_f)*1000 / 0xFFFFFFFFULL;

	remote = remote_s;
	remote *= 1000;
	remote += ((u64) remote_f)*1000 / 0xFFFFFFFFULL;

	return (s32) (local - remote);
}