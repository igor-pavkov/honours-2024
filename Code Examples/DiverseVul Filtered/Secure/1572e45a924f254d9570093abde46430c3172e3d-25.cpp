static int __perf_event_overflow(struct perf_event *event,
				   int throttle, struct perf_sample_data *data,
				   struct pt_regs *regs)
{
	int events = atomic_read(&event->event_limit);
	int ret = 0;

	/*
	 * Non-sampling counters might still use the PMI to fold short
	 * hardware counters, ignore those.
	 */
	if (unlikely(!is_sampling_event(event)))
		return 0;

	ret = __perf_event_account_interrupt(event, throttle);

	/*
	 * XXX event_limit might not quite work as expected on inherited
	 * events
	 */

	event->pending_kill = POLL_IN;
	if (events && atomic_dec_and_test(&event->event_limit)) {
		ret = 1;
		event->pending_kill = POLL_HUP;

		perf_event_disable_inatomic(event);
	}

	READ_ONCE(event->overflow_handler)(event, data, regs);

	if (*perf_event_fasync(event) && event->pending_kill) {
		event->pending_wakeup = 1;
		irq_work_queue(&event->pending);
	}

	return ret;
}