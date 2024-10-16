static inline void hrtimer_init_hres(struct hrtimer_cpu_base *base)
{
	base->expires_next.tv64 = KTIME_MAX;
	base->hres_active = 0;
	INIT_LIST_HEAD(&base->cb_pending);
}