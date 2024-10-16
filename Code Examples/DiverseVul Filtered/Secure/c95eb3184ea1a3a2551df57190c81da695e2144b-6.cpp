static irqreturn_t armpmu_dispatch_irq(int irq, void *dev)
{
	struct arm_pmu *armpmu = (struct arm_pmu *) dev;
	struct platform_device *plat_device = armpmu->plat_device;
	struct arm_pmu_platdata *plat = dev_get_platdata(&plat_device->dev);

	if (plat && plat->handle_irq)
		return plat->handle_irq(irq, dev, armpmu->handle_irq);
	else
		return armpmu->handle_irq(irq, dev);
}