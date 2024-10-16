int kvm_arch_init_vm(struct kvm *kvm, unsigned long type)
{
	struct kvmppc_ops *kvm_ops = NULL;
	/*
	 * if we have both HV and PR enabled, default is HV
	 */
	if (type == 0) {
		if (kvmppc_hv_ops)
			kvm_ops = kvmppc_hv_ops;
		else
			kvm_ops = kvmppc_pr_ops;
		if (!kvm_ops)
			goto err_out;
	} else	if (type == KVM_VM_PPC_HV) {
		if (!kvmppc_hv_ops)
			goto err_out;
		kvm_ops = kvmppc_hv_ops;
	} else if (type == KVM_VM_PPC_PR) {
		if (!kvmppc_pr_ops)
			goto err_out;
		kvm_ops = kvmppc_pr_ops;
	} else
		goto err_out;

	if (kvm_ops->owner && !try_module_get(kvm_ops->owner))
		return -ENOENT;

	kvm->arch.kvm_ops = kvm_ops;
	return kvmppc_core_init_vm(kvm);
err_out:
	return -EINVAL;
}