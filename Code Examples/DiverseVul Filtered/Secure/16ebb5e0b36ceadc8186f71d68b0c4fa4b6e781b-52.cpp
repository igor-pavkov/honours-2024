static enum hrtimer_restart qdisc_watchdog(struct hrtimer *timer)
{
	struct qdisc_watchdog *wd = container_of(timer, struct qdisc_watchdog,
						 timer);

	wd->qdisc->flags &= ~TCQ_F_THROTTLED;
	__netif_schedule(qdisc_root(wd->qdisc));

	return HRTIMER_NORESTART;
}