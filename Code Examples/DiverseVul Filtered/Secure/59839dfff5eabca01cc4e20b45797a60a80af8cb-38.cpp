int kvm_load_segment_descriptor(struct kvm_vcpu *vcpu, u16 selector,
				int type_bits, int seg)
{
	struct kvm_segment kvm_seg;

	if (!(vcpu->arch.cr0 & X86_CR0_PE))
		return kvm_load_realmode_segment(vcpu, selector, seg);
	if (load_segment_descriptor_to_kvm_desct(vcpu, selector, &kvm_seg))
		return 1;
	kvm_seg.type |= type_bits;

	if (seg != VCPU_SREG_SS && seg != VCPU_SREG_CS &&
	    seg != VCPU_SREG_LDTR)
		if (!kvm_seg.s)
			kvm_seg.unusable = 1;

	kvm_set_segment(vcpu, &kvm_seg, seg);
	return 0;
}