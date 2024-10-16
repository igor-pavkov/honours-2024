static int exclusive_event_init(struct perf_event *event)
{
	struct pmu *pmu = event->pmu;

	if (!is_exclusive_pmu(pmu))
		return 0;

	/*
	 * Prevent co-existence of per-task and cpu-wide events on the
	 * same exclusive pmu.
	 *
	 * Negative pmu::exclusive_cnt means there are cpu-wide
	 * events on this "exclusive" pmu, positive means there are
	 * per-task events.
	 *
	 * Since this is called in perf_event_alloc() path, event::ctx
	 * doesn't exist yet; it is, however, safe to use PERF_ATTACH_TASK
	 * to mean "per-task event", because unlike other attach states it
	 * never gets cleared.
	 */
	if (event->attach_state & PERF_ATTACH_TASK) {
		if (!atomic_inc_unless_negative(&pmu->exclusive_cnt))
			return -EBUSY;
	} else {
		if (!atomic_dec_unless_positive(&pmu->exclusive_cnt))
			return -EBUSY;
	}

	return 0;
}