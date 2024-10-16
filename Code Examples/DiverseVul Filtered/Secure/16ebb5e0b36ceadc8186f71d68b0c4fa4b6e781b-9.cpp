void qdisc_watchdog_schedule(struct qdisc_watchdog *wd, psched_time_t expires)
{
	ktime_t time;

	if (test_bit(__QDISC_STATE_DEACTIVATED,
		     &qdisc_root_sleeping(wd->qdisc)->state))
		return;

	wd->qdisc->flags |= TCQ_F_THROTTLED;
	time = ktime_set(0, 0);
	time = ktime_add_ns(time, PSCHED_TICKS2NS(expires));
	hrtimer_start(&wd->timer, time, HRTIMER_MODE_ABS);
}