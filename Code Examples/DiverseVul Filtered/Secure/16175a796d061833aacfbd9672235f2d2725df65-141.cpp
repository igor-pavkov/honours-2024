static inline int cpu_has_vmx_ept(void)
{
	return (vmcs_config.cpu_based_2nd_exec_ctrl &
		SECONDARY_EXEC_ENABLE_EPT);
}