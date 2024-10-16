void dvb_frontend_sleep_until(ktime_t *waketime, u32 add_usec)
{
	s32 delta;

	*waketime = ktime_add_us(*waketime, add_usec);
	delta = ktime_us_delta(ktime_get_boottime(), *waketime);
	if (delta > 2500) {
		msleep((delta - 1500) / 1000);
		delta = ktime_us_delta(ktime_get_boottime(), *waketime);
	}
	if (delta > 0)
		udelay(delta);
}