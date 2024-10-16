static void check_ept_pointer_match(struct kvm *kvm)
{
	struct kvm_vcpu *vcpu;
	u64 tmp_eptp = INVALID_PAGE;
	int i;

	kvm_for_each_vcpu(i, vcpu, kvm) {
		if (!VALID_PAGE(tmp_eptp)) {
			tmp_eptp = to_vmx(vcpu)->ept_pointer;
		} else if (tmp_eptp != to_vmx(vcpu)->ept_pointer) {
			to_kvm_vmx(kvm)->ept_pointers_match
				= EPT_POINTERS_MISMATCH;
			return;
		}
	}

	to_kvm_vmx(kvm)->ept_pointers_match = EPT_POINTERS_MATCH;
}