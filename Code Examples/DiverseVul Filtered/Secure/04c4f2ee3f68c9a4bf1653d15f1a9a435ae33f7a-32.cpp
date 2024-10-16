static __init void vmx_set_cpu_caps(void)
{
	kvm_set_cpu_caps();

	/* CPUID 0x1 */
	if (nested)
		kvm_cpu_cap_set(X86_FEATURE_VMX);

	/* CPUID 0x7 */
	if (kvm_mpx_supported())
		kvm_cpu_cap_check_and_set(X86_FEATURE_MPX);
	if (!cpu_has_vmx_invpcid())
		kvm_cpu_cap_clear(X86_FEATURE_INVPCID);
	if (vmx_pt_mode_is_host_guest())
		kvm_cpu_cap_check_and_set(X86_FEATURE_INTEL_PT);

	if (vmx_umip_emulated())
		kvm_cpu_cap_set(X86_FEATURE_UMIP);

	/* CPUID 0xD.1 */
	supported_xss = 0;
	if (!cpu_has_vmx_xsaves())
		kvm_cpu_cap_clear(X86_FEATURE_XSAVES);

	/* CPUID 0x80000001 */
	if (!cpu_has_vmx_rdtscp())
		kvm_cpu_cap_clear(X86_FEATURE_RDTSCP);

	if (cpu_has_vmx_waitpkg())
		kvm_cpu_cap_check_and_set(X86_FEATURE_WAITPKG);
}