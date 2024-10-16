bool kvm_vcpu_eligible_for_directed_yield(struct kvm_vcpu *vcpu)
{
	bool eligible;

	eligible = !vcpu->spin_loop.in_spin_loop ||
			(vcpu->spin_loop.in_spin_loop &&
			 vcpu->spin_loop.dy_eligible);

	if (vcpu->spin_loop.in_spin_loop)
		kvm_vcpu_set_dy_eligible(vcpu, !vcpu->spin_loop.dy_eligible);

	return eligible;
}