struct kvm_vcpu *kvm_arch_vcpu_create(struct kvm *kvm,
						unsigned int id)
{
	return kvm_x86_ops->vcpu_create(kvm, id);
}