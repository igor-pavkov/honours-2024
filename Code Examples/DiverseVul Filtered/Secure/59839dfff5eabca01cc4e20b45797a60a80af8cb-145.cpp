void kvm_load_guest_fpu(struct kvm_vcpu *vcpu)
{
	if (!vcpu->fpu_active || vcpu->guest_fpu_loaded)
		return;

	vcpu->guest_fpu_loaded = 1;
	kvm_fx_save(&vcpu->arch.host_fx_image);
	kvm_fx_restore(&vcpu->arch.guest_fx_image);
}