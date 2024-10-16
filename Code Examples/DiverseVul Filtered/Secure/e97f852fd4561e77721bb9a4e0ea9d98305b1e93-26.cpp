static inline int complete_emulated_io(struct kvm_vcpu *vcpu)
{
	int r;
	vcpu->srcu_idx = srcu_read_lock(&vcpu->kvm->srcu);
	r = kvm_emulate_instruction(vcpu, EMULTYPE_NO_DECODE);
	srcu_read_unlock(&vcpu->kvm->srcu, vcpu->srcu_idx);
	if (r != EMULATE_DONE)
		return 0;
	return 1;
}