static int armpmu_runtime_resume(struct device *dev)
{
	struct arm_pmu_platdata *plat = dev_get_platdata(dev);

	if (plat && plat->runtime_resume)
		return plat->runtime_resume(dev);

	return 0;
}