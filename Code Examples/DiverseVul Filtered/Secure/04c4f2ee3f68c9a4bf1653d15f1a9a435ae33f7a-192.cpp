static void vmx_vcpu_after_set_cpuid(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	/* xsaves_enabled is recomputed in vmx_compute_secondary_exec_control(). */
	vcpu->arch.xsaves_enabled = false;

	if (cpu_has_secondary_exec_ctrls()) {
		vmx_compute_secondary_exec_control(vmx);
		vmcs_set_secondary_exec_control(vmx);
	}

	if (nested_vmx_allowed(vcpu))
		to_vmx(vcpu)->msr_ia32_feature_control_valid_bits |=
			FEAT_CTL_VMX_ENABLED_INSIDE_SMX |
			FEAT_CTL_VMX_ENABLED_OUTSIDE_SMX;
	else
		to_vmx(vcpu)->msr_ia32_feature_control_valid_bits &=
			~(FEAT_CTL_VMX_ENABLED_INSIDE_SMX |
			  FEAT_CTL_VMX_ENABLED_OUTSIDE_SMX);

	if (nested_vmx_allowed(vcpu)) {
		nested_vmx_cr_fixed1_bits_update(vcpu);
		nested_vmx_entry_exit_ctls_update(vcpu);
	}

	if (boot_cpu_has(X86_FEATURE_INTEL_PT) &&
			guest_cpuid_has(vcpu, X86_FEATURE_INTEL_PT))
		update_intel_pt_cfg(vcpu);

	if (boot_cpu_has(X86_FEATURE_RTM)) {
		struct vmx_uret_msr *msr;
		msr = vmx_find_uret_msr(vmx, MSR_IA32_TSX_CTRL);
		if (msr) {
			bool enabled = guest_cpuid_has(vcpu, X86_FEATURE_RTM);
			vmx_set_guest_uret_msr(vmx, msr, enabled ? 0 : TSX_CTRL_RTM_DISABLE);
		}
	}

	set_cr4_guest_host_mask(vmx);

	/* Refresh #PF interception to account for MAXPHYADDR changes. */
	vmx_update_exception_bitmap(vcpu);
}