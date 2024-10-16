static inline void hrtimer_init_timer_hres(struct hrtimer *timer)
{
	INIT_LIST_HEAD(&timer->cb_entry);
}