struct kvm_vcpu *kvm_arch_vcpu_create(struct kvm *kvm, unsigned int id)
{
	struct kvm_vcpu *vcpu;
	vcpu = kvmppc_core_vcpu_create(kvm, id);
	if (!IS_ERR(vcpu)) {
		vcpu->arch.wqp = &vcpu->wq;
		kvmppc_create_vcpu_debugfs(vcpu, id);
	}
	return vcpu;
}