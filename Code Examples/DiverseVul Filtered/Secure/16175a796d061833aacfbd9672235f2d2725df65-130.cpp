static inline bool cpu_has_vmx_virtualize_apic_accesses(void)
{
	return flexpriority_enabled
		&& (vmcs_config.cpu_based_2nd_exec_ctrl &
		    SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES);
}