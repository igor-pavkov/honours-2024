static __init int vmx_disabled_by_bios(void)
{
	u64 msr;

	rdmsrl(MSR_IA32_FEATURE_CONTROL, msr);
	return (msr & (FEATURE_CONTROL_LOCKED |
		       FEATURE_CONTROL_VMXON_ENABLED))
	    == FEATURE_CONTROL_LOCKED;
	/* locked but not enabled */
}