static int hv_remote_flush_tlb_with_range(struct kvm *kvm,
		struct kvm_tlb_range *range)
{
	struct kvm_vcpu *vcpu;
	int ret = 0, i;

	spin_lock(&to_kvm_vmx(kvm)->ept_pointer_lock);

	if (to_kvm_vmx(kvm)->ept_pointers_match == EPT_POINTERS_CHECK)
		check_ept_pointer_match(kvm);

	if (to_kvm_vmx(kvm)->ept_pointers_match != EPT_POINTERS_MATCH) {
		kvm_for_each_vcpu(i, vcpu, kvm) {
			/* If ept_pointer is invalid pointer, bypass flush request. */
			if (VALID_PAGE(to_vmx(vcpu)->ept_pointer))
				ret |= __hv_remote_flush_tlb_with_range(
					kvm, vcpu, range);
		}
	} else {
		ret = __hv_remote_flush_tlb_with_range(kvm,
				kvm_get_vcpu(kvm, 0), range);
	}

	spin_unlock(&to_kvm_vmx(kvm)->ept_pointer_lock);
	return ret;
}