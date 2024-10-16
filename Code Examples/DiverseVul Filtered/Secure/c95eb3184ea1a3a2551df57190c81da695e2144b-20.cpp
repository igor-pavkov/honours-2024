static void armpmu_enable(struct pmu *pmu)
{
	struct arm_pmu *armpmu = to_arm_pmu(pmu);
	struct pmu_hw_events *hw_events = armpmu->get_hw_events();
	int enabled = bitmap_weight(hw_events->used_mask, armpmu->num_events);

	if (enabled)
		armpmu->start(armpmu);
}