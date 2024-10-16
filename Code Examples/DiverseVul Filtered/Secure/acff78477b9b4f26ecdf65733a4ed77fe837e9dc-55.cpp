static int prepare_vmcs02(struct kvm_vcpu *vcpu, struct vmcs12 *vmcs12,
			  u32 *entry_failure_code)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct hv_enlightened_vmcs *hv_evmcs = vmx->nested.hv_evmcs;

	if (vmx->nested.dirty_vmcs12 || vmx->nested.hv_evmcs) {
		prepare_vmcs02_full(vmx, vmcs12);
		vmx->nested.dirty_vmcs12 = false;
	}

	/*
	 * First, the fields that are shadowed.  This must be kept in sync
	 * with vmcs_shadow_fields.h.
	 */
	if (!hv_evmcs || !(hv_evmcs->hv_clean_fields &
			   HV_VMX_ENLIGHTENED_CLEAN_FIELD_GUEST_GRP2)) {
		vmcs_write32(GUEST_CS_AR_BYTES, vmcs12->guest_cs_ar_bytes);
		vmcs_write32(GUEST_SS_AR_BYTES, vmcs12->guest_ss_ar_bytes);
	}

	if (vmx->nested.nested_run_pending &&
	    (vmcs12->vm_entry_controls & VM_ENTRY_LOAD_DEBUG_CONTROLS)) {
		kvm_set_dr(vcpu, 7, vmcs12->guest_dr7);
		vmcs_write64(GUEST_IA32_DEBUGCTL, vmcs12->guest_ia32_debugctl);
	} else {
		kvm_set_dr(vcpu, 7, vcpu->arch.dr7);
		vmcs_write64(GUEST_IA32_DEBUGCTL, vmx->nested.vmcs01_debugctl);
	}
	vmx_set_rflags(vcpu, vmcs12->guest_rflags);

	/* EXCEPTION_BITMAP and CR0_GUEST_HOST_MASK should basically be the
	 * bitwise-or of what L1 wants to trap for L2, and what we want to
	 * trap. Note that CR0.TS also needs updating - we do this later.
	 */
	update_exception_bitmap(vcpu);
	vcpu->arch.cr0_guest_owned_bits &= ~vmcs12->cr0_guest_host_mask;
	vmcs_writel(CR0_GUEST_HOST_MASK, ~vcpu->arch.cr0_guest_owned_bits);

	if (vmx->nested.nested_run_pending &&
	    (vmcs12->vm_entry_controls & VM_ENTRY_LOAD_IA32_PAT)) {
		vmcs_write64(GUEST_IA32_PAT, vmcs12->guest_ia32_pat);
		vcpu->arch.pat = vmcs12->guest_ia32_pat;
	} else if (vmcs_config.vmentry_ctrl & VM_ENTRY_LOAD_IA32_PAT) {
		vmcs_write64(GUEST_IA32_PAT, vmx->vcpu.arch.pat);
	}

	vmcs_write64(TSC_OFFSET, vcpu->arch.tsc_offset);

	if (kvm_has_tsc_control)
		decache_tsc_multiplier(vmx);

	if (enable_vpid) {
		/*
		 * There is no direct mapping between vpid02 and vpid12, the
		 * vpid02 is per-vCPU for L0 and reused while the value of
		 * vpid12 is changed w/ one invvpid during nested vmentry.
		 * The vpid12 is allocated by L1 for L2, so it will not
		 * influence global bitmap(for vpid01 and vpid02 allocation)
		 * even if spawn a lot of nested vCPUs.
		 */
		if (nested_cpu_has_vpid(vmcs12) && nested_has_guest_tlb_tag(vcpu)) {
			if (vmcs12->virtual_processor_id != vmx->nested.last_vpid) {
				vmx->nested.last_vpid = vmcs12->virtual_processor_id;
				__vmx_flush_tlb(vcpu, nested_get_vpid02(vcpu), false);
			}
		} else {
			/*
			 * If L1 use EPT, then L0 needs to execute INVEPT on
			 * EPTP02 instead of EPTP01. Therefore, delay TLB
			 * flush until vmcs02->eptp is fully updated by
			 * KVM_REQ_LOAD_CR3. Note that this assumes
			 * KVM_REQ_TLB_FLUSH is evaluated after
			 * KVM_REQ_LOAD_CR3 in vcpu_enter_guest().
			 */
			kvm_make_request(KVM_REQ_TLB_FLUSH, vcpu);
		}
	}

	if (nested_cpu_has_ept(vmcs12))
		nested_ept_init_mmu_context(vcpu);
	else if (nested_cpu_has2(vmcs12,
				 SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES))
		vmx_flush_tlb(vcpu, true);

	/*
	 * This sets GUEST_CR0 to vmcs12->guest_cr0, possibly modifying those
	 * bits which we consider mandatory enabled.
	 * The CR0_READ_SHADOW is what L2 should have expected to read given
	 * the specifications by L1; It's not enough to take
	 * vmcs12->cr0_read_shadow because on our cr0_guest_host_mask we we
	 * have more bits than L1 expected.
	 */
	vmx_set_cr0(vcpu, vmcs12->guest_cr0);
	vmcs_writel(CR0_READ_SHADOW, nested_read_cr0(vmcs12));

	vmx_set_cr4(vcpu, vmcs12->guest_cr4);
	vmcs_writel(CR4_READ_SHADOW, nested_read_cr4(vmcs12));

	vcpu->arch.efer = nested_vmx_calc_efer(vmx, vmcs12);
	/* Note: may modify VM_ENTRY/EXIT_CONTROLS and GUEST/HOST_IA32_EFER */
	vmx_set_efer(vcpu, vcpu->arch.efer);

	/*
	 * Guest state is invalid and unrestricted guest is disabled,
	 * which means L1 attempted VMEntry to L2 with invalid state.
	 * Fail the VMEntry.
	 */
	if (vmx->emulation_required) {
		*entry_failure_code = ENTRY_FAIL_DEFAULT;
		return 1;
	}

	/* Shadow page tables on either EPT or shadow page tables. */
	if (nested_vmx_load_cr3(vcpu, vmcs12->guest_cr3, nested_cpu_has_ept(vmcs12),
				entry_failure_code))
		return 1;

	if (!enable_ept)
		vcpu->arch.walk_mmu->inject_page_fault = vmx_inject_page_fault_nested;

	kvm_register_write(vcpu, VCPU_REGS_RSP, vmcs12->guest_rsp);
	kvm_register_write(vcpu, VCPU_REGS_RIP, vmcs12->guest_rip);
	return 0;
}