static void armpmu_init(struct arm_pmu *armpmu)
{
	atomic_set(&armpmu->active_events, 0);
	mutex_init(&armpmu->reserve_mutex);

	armpmu->pmu = (struct pmu) {
		.pmu_enable	= armpmu_enable,
		.pmu_disable	= armpmu_disable,
		.event_init	= armpmu_event_init,
		.add		= armpmu_add,
		.del		= armpmu_del,
		.start		= armpmu_start,
		.stop		= armpmu_stop,
		.read		= armpmu_read,
	};
}