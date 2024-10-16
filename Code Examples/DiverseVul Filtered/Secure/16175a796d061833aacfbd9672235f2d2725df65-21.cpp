static inline int cpu_has_vmx_tpr_shadow(void)
{
	return (vmcs_config.cpu_based_exec_ctrl & CPU_BASED_TPR_SHADOW);
}