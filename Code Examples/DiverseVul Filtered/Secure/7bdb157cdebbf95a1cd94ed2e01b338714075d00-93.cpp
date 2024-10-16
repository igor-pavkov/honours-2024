static void calc_timer_values(struct perf_event *event,
				u64 *now,
				u64 *enabled,
				u64 *running)
{
	u64 ctx_time;

	*now = perf_clock();
	ctx_time = event->shadow_ctx_time + *now;
	__perf_update_times(event, ctx_time, enabled, running);
}