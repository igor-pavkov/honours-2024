static inline int cpu_has_vmx_msr_bitmap(void)
{
	return (vmcs_config.cpu_based_exec_ctrl & CPU_BASED_USE_MSR_BITMAPS);
}