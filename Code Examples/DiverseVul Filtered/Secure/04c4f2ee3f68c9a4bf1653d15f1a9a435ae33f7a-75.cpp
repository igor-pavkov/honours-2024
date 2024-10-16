static void vmx_compute_secondary_exec_control(struct vcpu_vmx *vmx)
{
	struct kvm_vcpu *vcpu = &vmx->vcpu;

	u32 exec_control = vmcs_config.cpu_based_2nd_exec_ctrl;

	if (vmx_pt_mode_is_system())
		exec_control &= ~(SECONDARY_EXEC_PT_USE_GPA | SECONDARY_EXEC_PT_CONCEAL_VMX);
	if (!cpu_need_virtualize_apic_accesses(vcpu))
		exec_control &= ~SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES;
	if (vmx->vpid == 0)
		exec_control &= ~SECONDARY_EXEC_ENABLE_VPID;
	if (!enable_ept) {
		exec_control &= ~SECONDARY_EXEC_ENABLE_EPT;
		enable_unrestricted_guest = 0;
	}
	if (!enable_unrestricted_guest)
		exec_control &= ~SECONDARY_EXEC_UNRESTRICTED_GUEST;
	if (kvm_pause_in_guest(vmx->vcpu.kvm))
		exec_control &= ~SECONDARY_EXEC_PAUSE_LOOP_EXITING;
	if (!kvm_vcpu_apicv_active(vcpu))
		exec_control &= ~(SECONDARY_EXEC_APIC_REGISTER_VIRT |
				  SECONDARY_EXEC_VIRTUAL_INTR_DELIVERY);
	exec_control &= ~SECONDARY_EXEC_VIRTUALIZE_X2APIC_MODE;

	/* SECONDARY_EXEC_DESC is enabled/disabled on writes to CR4.UMIP,
	 * in vmx_set_cr4.  */
	exec_control &= ~SECONDARY_EXEC_DESC;

	/* SECONDARY_EXEC_SHADOW_VMCS is enabled when L1 executes VMPTRLD
	   (handle_vmptrld).
	   We can NOT enable shadow_vmcs here because we don't have yet
	   a current VMCS12
	*/
	exec_control &= ~SECONDARY_EXEC_SHADOW_VMCS;

	/*
	 * PML is enabled/disabled when dirty logging of memsmlots changes, but
	 * it needs to be set here when dirty logging is already active, e.g.
	 * if this vCPU was created after dirty logging was enabled.
	 */
	if (!vcpu->kvm->arch.cpu_dirty_logging_count)
		exec_control &= ~SECONDARY_EXEC_ENABLE_PML;

	if (cpu_has_vmx_xsaves()) {
		/* Exposing XSAVES only when XSAVE is exposed */
		bool xsaves_enabled =
			boot_cpu_has(X86_FEATURE_XSAVE) &&
			guest_cpuid_has(vcpu, X86_FEATURE_XSAVE) &&
			guest_cpuid_has(vcpu, X86_FEATURE_XSAVES);

		vcpu->arch.xsaves_enabled = xsaves_enabled;

		vmx_adjust_secondary_exec_control(vmx, &exec_control,
						  SECONDARY_EXEC_XSAVES,
						  xsaves_enabled, false);
	}

	vmx_adjust_sec_exec_feature(vmx, &exec_control, rdtscp, RDTSCP);
	vmx_adjust_sec_exec_feature(vmx, &exec_control, invpcid, INVPCID);

	vmx_adjust_sec_exec_exiting(vmx, &exec_control, rdrand, RDRAND);
	vmx_adjust_sec_exec_exiting(vmx, &exec_control, rdseed, RDSEED);

	vmx_adjust_sec_exec_control(vmx, &exec_control, waitpkg, WAITPKG,
				    ENABLE_USR_WAIT_PAUSE, false);

	if (!vcpu->kvm->arch.bus_lock_detection_enabled)
		exec_control &= ~SECONDARY_EXEC_BUS_LOCK_DETECTION;

	vmx->secondary_exec_control = exec_control;
}