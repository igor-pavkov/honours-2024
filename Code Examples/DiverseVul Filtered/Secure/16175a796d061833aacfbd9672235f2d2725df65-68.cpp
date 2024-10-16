static void update_tpr_threshold(struct kvm_vcpu *vcpu)
{
	int max_irr, tpr;

	if (!vm_need_tpr_shadow(vcpu->kvm))
		return;

	if (!kvm_lapic_enabled(vcpu) ||
	    ((max_irr = kvm_lapic_find_highest_irr(vcpu)) == -1)) {
		vmcs_write32(TPR_THRESHOLD, 0);
		return;
	}

	tpr = (kvm_lapic_get_cr8(vcpu) & 0x0f) << 4;
	vmcs_write32(TPR_THRESHOLD, (max_irr > tpr) ? tpr >> 4 : max_irr >> 4);
}