void gf_net_get_ntp(u32 *sec, u32 *frac)
{
	u64 frac_part;
	struct timeval now;
	gettimeofday(&now, NULL);
	if (sec) {
		*sec = (u32) (now.tv_sec) + ntp_shift;
	}

	if (frac) {
		frac_part = now.tv_usec * 0xFFFFFFFFULL;
		frac_part /= 1000000;
		*frac = (u32) ( frac_part );
	}
}