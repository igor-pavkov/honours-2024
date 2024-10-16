static int armpmu_event_init(struct perf_event *event)
{
	struct arm_pmu *armpmu = to_arm_pmu(event->pmu);
	int err = 0;
	atomic_t *active_events = &armpmu->active_events;

	/* does not support taken branch sampling */
	if (has_branch_stack(event))
		return -EOPNOTSUPP;

	if (armpmu->map_event(event) == -ENOENT)
		return -ENOENT;

	event->destroy = hw_perf_event_destroy;

	if (!atomic_inc_not_zero(active_events)) {
		mutex_lock(&armpmu->reserve_mutex);
		if (atomic_read(active_events) == 0)
			err = armpmu_reserve_hardware(armpmu);

		if (!err)
			atomic_inc(active_events);
		mutex_unlock(&armpmu->reserve_mutex);
	}

	if (err)
		return err;

	err = __hw_perf_event_init(event);
	if (err)
		hw_perf_event_destroy(event);

	return err;
}