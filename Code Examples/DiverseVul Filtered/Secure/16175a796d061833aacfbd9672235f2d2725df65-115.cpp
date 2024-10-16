static inline int cpu_has_secondary_exec_ctrls(void)
{
	return (vmcs_config.cpu_based_exec_ctrl &
		CPU_BASED_ACTIVATE_SECONDARY_CONTROLS);
}