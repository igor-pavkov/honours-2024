static void setup_msrs(struct vcpu_vmx *vmx)
{
	vmx->guest_uret_msrs_loaded = false;
	vmx->nr_active_uret_msrs = 0;
#ifdef CONFIG_X86_64
	/*
	 * The SYSCALL MSRs are only needed on long mode guests, and only
	 * when EFER.SCE is set.
	 */
	if (is_long_mode(&vmx->vcpu) && (vmx->vcpu.arch.efer & EFER_SCE)) {
		vmx_setup_uret_msr(vmx, MSR_STAR);
		vmx_setup_uret_msr(vmx, MSR_LSTAR);
		vmx_setup_uret_msr(vmx, MSR_SYSCALL_MASK);
	}
#endif
	if (update_transition_efer(vmx))
		vmx_setup_uret_msr(vmx, MSR_EFER);

	if (guest_cpuid_has(&vmx->vcpu, X86_FEATURE_RDTSCP))
		vmx_setup_uret_msr(vmx, MSR_TSC_AUX);

	vmx_setup_uret_msr(vmx, MSR_IA32_TSX_CTRL);

	if (cpu_has_vmx_msr_bitmap())
		vmx_update_msr_bitmap(&vmx->vcpu);
}