static int armpmu_runtime_suspend(struct device *dev)
{
	struct arm_pmu_platdata *plat = dev_get_platdata(dev);

	if (plat && plat->runtime_suspend)
		return plat->runtime_suspend(dev);

	return 0;
}