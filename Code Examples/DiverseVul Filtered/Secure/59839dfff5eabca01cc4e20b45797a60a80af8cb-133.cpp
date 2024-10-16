static int kvm_vcpu_ioctl_nmi(struct kvm_vcpu *vcpu)
{
	vcpu_load(vcpu);
	kvm_inject_nmi(vcpu);
	vcpu_put(vcpu);

	return 0;
}