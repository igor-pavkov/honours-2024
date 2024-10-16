void qdisc_watchdog_cancel(struct qdisc_watchdog *wd)
{
	hrtimer_cancel(&wd->timer);
	wd->qdisc->flags &= ~TCQ_F_THROTTLED;
}