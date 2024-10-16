static inline int hrtimer_cb_pending(const struct hrtimer *timer)
{
	return timer->state & HRTIMER_STATE_PENDING;
}