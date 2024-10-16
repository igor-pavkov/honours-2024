static int hv_enable_direct_tlbflush(struct kvm_vcpu *vcpu)
{
	struct hv_enlightened_vmcs *evmcs;
	struct hv_partition_assist_pg **p_hv_pa_pg =
			&to_kvm_hv(vcpu->kvm)->hv_pa_pg;
	/*
	 * Synthetic VM-Exit is not enabled in current code and so All
	 * evmcs in singe VM shares same assist page.
	 */
	if (!*p_hv_pa_pg)
		*p_hv_pa_pg = kzalloc(PAGE_SIZE, GFP_KERNEL);

	if (!*p_hv_pa_pg)
		return -ENOMEM;

	evmcs = (struct hv_enlightened_vmcs *)to_vmx(vcpu)->loaded_vmcs->vmcs;

	evmcs->partition_assist_page =
		__pa(*p_hv_pa_pg);
	evmcs->hv_vm_id = (unsigned long)vcpu->kvm;
	evmcs->hv_enlightenments_control.nested_flush_hypercall = 1;

	return 0;
}