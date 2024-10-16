void kvm_put_guest_fpu(struct kvm_vcpu *vcpu)
{
	if (!vcpu->guest_fpu_loaded)
		return;

	vcpu->guest_fpu_loaded = 0;
	kvm_fx_save(&vcpu->arch.guest_fx_image);
	kvm_fx_restore(&vcpu->arch.host_fx_image);
	++vcpu->stat.fpu_reload;
}