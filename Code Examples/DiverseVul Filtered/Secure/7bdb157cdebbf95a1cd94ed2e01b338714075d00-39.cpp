static void exclusive_event_destroy(struct perf_event *event)
{
	struct pmu *pmu = event->pmu;

	if (!is_exclusive_pmu(pmu))
		return;

	/* see comment in exclusive_event_init() */
	if (event->attach_state & PERF_ATTACH_TASK)
		atomic_dec(&pmu->exclusive_cnt);
	else
		atomic_inc(&pmu->exclusive_cnt);
}