static int _perf_event_period(struct perf_event *event, u64 value)
{
	if (!is_sampling_event(event))
		return -EINVAL;

	if (!value)
		return -EINVAL;

	if (event->attr.freq && value > sysctl_perf_event_sample_rate)
		return -EINVAL;

	if (perf_event_check_period(event, value))
		return -EINVAL;

	if (!event->attr.freq && (value & (1ULL << 63)))
		return -EINVAL;

	event_function_call(event, __perf_event_period, &value);

	return 0;
}