int kvm_arch_vcpu_setup(struct kvm_vcpu *vcpu)
{
	kvm_vcpu_mtrr_init(vcpu);
	vcpu_load(vcpu);
	kvm_vcpu_reset(vcpu, false);
	kvm_init_mmu(vcpu, false);
	vcpu_put(vcpu);
	return 0;
}