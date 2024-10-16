static void update_exception_bitmap(struct kvm_vcpu *vcpu)
{
	u32 eb;

	eb = (1u << PF_VECTOR) | (1u << UD_VECTOR);
	if (!vcpu->fpu_active)
		eb |= 1u << NM_VECTOR;
	if (vcpu->guest_debug & KVM_GUESTDBG_ENABLE) {
		if (vcpu->guest_debug &
		    (KVM_GUESTDBG_SINGLESTEP | KVM_GUESTDBG_USE_HW_BP))
			eb |= 1u << DB_VECTOR;
		if (vcpu->guest_debug & KVM_GUESTDBG_USE_SW_BP)
			eb |= 1u << BP_VECTOR;
	}
	if (vcpu->arch.rmode.active)
		eb = ~0;
	if (vm_need_ept())
		eb &= ~(1u << PF_VECTOR); /* bypass_guest_pf = 0 */
	vmcs_write32(EXCEPTION_BITMAP, eb);
}