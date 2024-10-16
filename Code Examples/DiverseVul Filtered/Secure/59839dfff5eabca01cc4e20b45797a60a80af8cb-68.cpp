void kvm_arch_vcpu_destroy(struct kvm_vcpu *vcpu)
{
	vcpu_load(vcpu);
	kvm_mmu_unload(vcpu);
	vcpu_put(vcpu);

	kvm_x86_ops->vcpu_free(vcpu);
}