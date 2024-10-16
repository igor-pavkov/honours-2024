int armpmu_register(struct arm_pmu *armpmu, int type)
{
	armpmu_init(armpmu);
	pm_runtime_enable(&armpmu->plat_device->dev);
	pr_info("enabled with %s PMU driver, %d counters available\n",
			armpmu->name, armpmu->num_events);
	return perf_pmu_register(&armpmu->pmu, armpmu->name, type);
}