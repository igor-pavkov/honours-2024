static int load_segment_descriptor_to_kvm_desct(struct kvm_vcpu *vcpu,
						u16 selector,
						struct kvm_segment *kvm_seg)
{
	struct desc_struct seg_desc;

	if (load_guest_segment_descriptor(vcpu, selector, &seg_desc))
		return 1;
	seg_desct_to_kvm_desct(&seg_desc, selector, kvm_seg);
	return 0;
}