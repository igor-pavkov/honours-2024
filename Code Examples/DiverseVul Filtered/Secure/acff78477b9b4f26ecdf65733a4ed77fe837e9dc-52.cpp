static void load_vmcs12_host_state(struct kvm_vcpu *vcpu,
				   struct vmcs12 *vmcs12)
{
	struct kvm_segment seg;
	u32 entry_failure_code;

	if (vmcs12->vm_exit_controls & VM_EXIT_LOAD_IA32_EFER)
		vcpu->arch.efer = vmcs12->host_ia32_efer;
	else if (vmcs12->vm_exit_controls & VM_EXIT_HOST_ADDR_SPACE_SIZE)
		vcpu->arch.efer |= (EFER_LMA | EFER_LME);
	else
		vcpu->arch.efer &= ~(EFER_LMA | EFER_LME);
	vmx_set_efer(vcpu, vcpu->arch.efer);

	kvm_register_write(vcpu, VCPU_REGS_RSP, vmcs12->host_rsp);
	kvm_register_write(vcpu, VCPU_REGS_RIP, vmcs12->host_rip);
	vmx_set_rflags(vcpu, X86_EFLAGS_FIXED);
	vmx_set_interrupt_shadow(vcpu, 0);

	/*
	 * Note that calling vmx_set_cr0 is important, even if cr0 hasn't
	 * actually changed, because vmx_set_cr0 refers to efer set above.
	 *
	 * CR0_GUEST_HOST_MASK is already set in the original vmcs01
	 * (KVM doesn't change it);
	 */
	vcpu->arch.cr0_guest_owned_bits = X86_CR0_TS;
	vmx_set_cr0(vcpu, vmcs12->host_cr0);

	/* Same as above - no reason to call set_cr4_guest_host_mask().  */
	vcpu->arch.cr4_guest_owned_bits = ~vmcs_readl(CR4_GUEST_HOST_MASK);
	vmx_set_cr4(vcpu, vmcs12->host_cr4);

	nested_ept_uninit_mmu_context(vcpu);

	/*
	 * Only PDPTE load can fail as the value of cr3 was checked on entry and
	 * couldn't have changed.
	 */
	if (nested_vmx_load_cr3(vcpu, vmcs12->host_cr3, false, &entry_failure_code))
		nested_vmx_abort(vcpu, VMX_ABORT_LOAD_HOST_PDPTE_FAIL);

	if (!enable_ept)
		vcpu->arch.walk_mmu->inject_page_fault = kvm_inject_page_fault;

	/*
	 * If vmcs01 doesn't use VPID, CPU flushes TLB on every
	 * VMEntry/VMExit. Thus, no need to flush TLB.
	 *
	 * If vmcs12 doesn't use VPID, L1 expects TLB to be
	 * flushed on every VMEntry/VMExit.
	 *
	 * Otherwise, we can preserve TLB entries as long as we are
	 * able to tag L1 TLB entries differently than L2 TLB entries.
	 *
	 * If vmcs12 uses EPT, we need to execute this flush on EPTP01
	 * and therefore we request the TLB flush to happen only after VMCS EPTP
	 * has been set by KVM_REQ_LOAD_CR3.
	 */
	if (enable_vpid &&
	    (!nested_cpu_has_vpid(vmcs12) || !nested_has_guest_tlb_tag(vcpu))) {
		kvm_make_request(KVM_REQ_TLB_FLUSH, vcpu);
	}

	vmcs_write32(GUEST_SYSENTER_CS, vmcs12->host_ia32_sysenter_cs);
	vmcs_writel(GUEST_SYSENTER_ESP, vmcs12->host_ia32_sysenter_esp);
	vmcs_writel(GUEST_SYSENTER_EIP, vmcs12->host_ia32_sysenter_eip);
	vmcs_writel(GUEST_IDTR_BASE, vmcs12->host_idtr_base);
	vmcs_writel(GUEST_GDTR_BASE, vmcs12->host_gdtr_base);
	vmcs_write32(GUEST_IDTR_LIMIT, 0xFFFF);
	vmcs_write32(GUEST_GDTR_LIMIT, 0xFFFF);

	/* If not VM_EXIT_CLEAR_BNDCFGS, the L2 value propagates to L1.  */
	if (vmcs12->vm_exit_controls & VM_EXIT_CLEAR_BNDCFGS)
		vmcs_write64(GUEST_BNDCFGS, 0);

	if (vmcs12->vm_exit_controls & VM_EXIT_LOAD_IA32_PAT) {
		vmcs_write64(GUEST_IA32_PAT, vmcs12->host_ia32_pat);
		vcpu->arch.pat = vmcs12->host_ia32_pat;
	}
	if (vmcs12->vm_exit_controls & VM_EXIT_LOAD_IA32_PERF_GLOBAL_CTRL)
		vmcs_write64(GUEST_IA32_PERF_GLOBAL_CTRL,
			vmcs12->host_ia32_perf_global_ctrl);

	/* Set L1 segment info according to Intel SDM
	    27.5.2 Loading Host Segment and Descriptor-Table Registers */
	seg = (struct kvm_segment) {
		.base = 0,
		.limit = 0xFFFFFFFF,
		.selector = vmcs12->host_cs_selector,
		.type = 11,
		.present = 1,
		.s = 1,
		.g = 1
	};
	if (vmcs12->vm_exit_controls & VM_EXIT_HOST_ADDR_SPACE_SIZE)
		seg.l = 1;
	else
		seg.db = 1;
	vmx_set_segment(vcpu, &seg, VCPU_SREG_CS);
	seg = (struct kvm_segment) {
		.base = 0,
		.limit = 0xFFFFFFFF,
		.type = 3,
		.present = 1,
		.s = 1,
		.db = 1,
		.g = 1
	};
	seg.selector = vmcs12->host_ds_selector;
	vmx_set_segment(vcpu, &seg, VCPU_SREG_DS);
	seg.selector = vmcs12->host_es_selector;
	vmx_set_segment(vcpu, &seg, VCPU_SREG_ES);
	seg.selector = vmcs12->host_ss_selector;
	vmx_set_segment(vcpu, &seg, VCPU_SREG_SS);
	seg.selector = vmcs12->host_fs_selector;
	seg.base = vmcs12->host_fs_base;
	vmx_set_segment(vcpu, &seg, VCPU_SREG_FS);
	seg.selector = vmcs12->host_gs_selector;
	seg.base = vmcs12->host_gs_base;
	vmx_set_segment(vcpu, &seg, VCPU_SREG_GS);
	seg = (struct kvm_segment) {
		.base = vmcs12->host_tr_base,
		.limit = 0x67,
		.selector = vmcs12->host_tr_selector,
		.type = 11,
		.present = 1
	};
	vmx_set_segment(vcpu, &seg, VCPU_SREG_TR);

	kvm_set_dr(vcpu, 7, 0x400);
	vmcs_write64(GUEST_IA32_DEBUGCTL, 0);

	if (cpu_has_vmx_msr_bitmap())
		vmx_update_msr_bitmap(vcpu);

	if (nested_vmx_load_msr(vcpu, vmcs12->vm_exit_msr_load_addr,
				vmcs12->vm_exit_msr_load_count))
		nested_vmx_abort(vcpu, VMX_ABORT_LOAD_HOST_MSR_FAIL);
}