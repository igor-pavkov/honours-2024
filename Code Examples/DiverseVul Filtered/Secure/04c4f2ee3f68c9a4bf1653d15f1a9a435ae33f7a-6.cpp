static void vmx_enable_nmi_window(struct kvm_vcpu *vcpu)
{
	if (!enable_vnmi ||
	    vmcs_read32(GUEST_INTERRUPTIBILITY_INFO) & GUEST_INTR_STATE_STI) {
		vmx_enable_irq_window(vcpu);
		return;
	}

	exec_controls_setbit(to_vmx(vcpu), CPU_BASED_NMI_WINDOW_EXITING);
}