static __init int vmx_disabled_by_bios(void)
{
	return !boot_cpu_has(X86_FEATURE_MSR_IA32_FEAT_CTL) ||
	       !boot_cpu_has(X86_FEATURE_VMX);
}