static void vmx_update_cr8_intercept(struct kvm_vcpu *vcpu, int tpr, int irr)
{
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);
	int tpr_threshold;

	if (is_guest_mode(vcpu) &&
		nested_cpu_has(vmcs12, CPU_BASED_TPR_SHADOW))
		return;

	tpr_threshold = (irr == -1 || tpr < irr) ? 0 : irr;
	if (is_guest_mode(vcpu))
		to_vmx(vcpu)->nested.l1_tpr_threshold = tpr_threshold;
	else
		vmcs_write32(TPR_THRESHOLD, tpr_threshold);
}