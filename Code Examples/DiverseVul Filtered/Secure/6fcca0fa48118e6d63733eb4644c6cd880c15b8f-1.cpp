void __init psi_init(void)
{
	if (!psi_enable) {
		static_branch_enable(&psi_disabled);
		return;
	}

	psi_period = jiffies_to_nsecs(PSI_FREQ);
	group_init(&psi_system);
}