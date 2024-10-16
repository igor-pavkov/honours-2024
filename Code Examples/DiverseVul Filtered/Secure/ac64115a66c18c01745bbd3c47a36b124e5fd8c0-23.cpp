bool kvm_arch_has_irq_bypass(void)
{
	return ((kvmppc_hv_ops && kvmppc_hv_ops->irq_bypass_add_producer) ||
		(kvmppc_pr_ops && kvmppc_pr_ops->irq_bypass_add_producer));
}