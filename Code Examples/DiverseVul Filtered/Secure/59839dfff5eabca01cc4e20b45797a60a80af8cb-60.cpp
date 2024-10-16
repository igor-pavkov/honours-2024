static int kvm_task_switch_32(struct kvm_vcpu *vcpu, u16 tss_selector,
		       u16 old_tss_sel, u32 old_tss_base,
		       struct desc_struct *nseg_desc)
{
	struct tss_segment_32 tss_segment_32;
	int ret = 0;

	if (kvm_read_guest(vcpu->kvm, old_tss_base, &tss_segment_32,
			   sizeof tss_segment_32))
		goto out;

	save_state_to_tss32(vcpu, &tss_segment_32);

	if (kvm_write_guest(vcpu->kvm, old_tss_base, &tss_segment_32,
			    sizeof tss_segment_32))
		goto out;

	if (kvm_read_guest(vcpu->kvm, get_tss_base_addr(vcpu, nseg_desc),
			   &tss_segment_32, sizeof tss_segment_32))
		goto out;

	if (old_tss_sel != 0xffff) {
		tss_segment_32.prev_task_link = old_tss_sel;

		if (kvm_write_guest(vcpu->kvm,
				    get_tss_base_addr(vcpu, nseg_desc),
				    &tss_segment_32.prev_task_link,
				    sizeof tss_segment_32.prev_task_link))
			goto out;
	}

	if (load_state_from_tss32(vcpu, &tss_segment_32))
		goto out;

	ret = 1;
out:
	return ret;
}