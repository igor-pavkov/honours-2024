static void vmx_update_window_states(struct kvm_vcpu *vcpu)
{
	u32 guest_intr = vmcs_read32(GUEST_INTERRUPTIBILITY_INFO);

	vcpu->arch.nmi_window_open =
		!(guest_intr & (GUEST_INTR_STATE_STI |
				GUEST_INTR_STATE_MOV_SS |
				GUEST_INTR_STATE_NMI));
	if (!cpu_has_virtual_nmis() && to_vmx(vcpu)->soft_vnmi_blocked)
		vcpu->arch.nmi_window_open = 0;

	vcpu->arch.interrupt_window_open =
		((vmcs_readl(GUEST_RFLAGS) & X86_EFLAGS_IF) &&
		 !(guest_intr & (GUEST_INTR_STATE_STI |
				 GUEST_INTR_STATE_MOV_SS)));
}