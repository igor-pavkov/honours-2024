static inline void hrtimer_remove_cb_pending(struct hrtimer *timer)
{
	list_del_init(&timer->cb_entry);
}