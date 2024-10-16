void vmx_update_exception_bitmap(struct kvm_vcpu *vcpu)
{
	u32 eb;

	eb = (1u << PF_VECTOR) | (1u << UD_VECTOR) | (1u << MC_VECTOR) |
	     (1u << DB_VECTOR) | (1u << AC_VECTOR);
	/*
	 * Guest access to VMware backdoor ports could legitimately
	 * trigger #GP because of TSS I/O permission bitmap.
	 * We intercept those #GP and allow access to them anyway
	 * as VMware does.
	 */
	if (enable_vmware_backdoor)
		eb |= (1u << GP_VECTOR);
	if ((vcpu->guest_debug &
	     (KVM_GUESTDBG_ENABLE | KVM_GUESTDBG_USE_SW_BP)) ==
	    (KVM_GUESTDBG_ENABLE | KVM_GUESTDBG_USE_SW_BP))
		eb |= 1u << BP_VECTOR;
	if (to_vmx(vcpu)->rmode.vm86_active)
		eb = ~0;
	if (!vmx_need_pf_intercept(vcpu))
		eb &= ~(1u << PF_VECTOR);

	/* When we are running a nested L2 guest and L1 specified for it a
	 * certain exception bitmap, we must trap the same exceptions and pass
	 * them to L1. When running L2, we will only handle the exceptions
	 * specified above if L1 did not want them.
	 */
	if (is_guest_mode(vcpu))
		eb |= get_vmcs12(vcpu)->exception_bitmap;
        else {
		/*
		 * If EPT is enabled, #PF is only trapped if MAXPHYADDR is mismatched
		 * between guest and host.  In that case we only care about present
		 * faults.  For vmcs02, however, PFEC_MASK and PFEC_MATCH are set in
		 * prepare_vmcs02_rare.
		 */
		bool selective_pf_trap = enable_ept && (eb & (1u << PF_VECTOR));
		int mask = selective_pf_trap ? PFERR_PRESENT_MASK : 0;
		vmcs_write32(PAGE_FAULT_ERROR_CODE_MASK, mask);
		vmcs_write32(PAGE_FAULT_ERROR_CODE_MATCH, mask);
	}

	vmcs_write32(EXCEPTION_BITMAP, eb);
}