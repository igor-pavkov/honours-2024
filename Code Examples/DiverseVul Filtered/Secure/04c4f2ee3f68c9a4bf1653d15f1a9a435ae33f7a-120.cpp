static bool vmx_apic_init_signal_blocked(struct kvm_vcpu *vcpu)
{
	return to_vmx(vcpu)->nested.vmxon && !is_guest_mode(vcpu);
}