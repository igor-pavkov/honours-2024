struct vmcs *alloc_vmcs_cpu(bool shadow, int cpu, gfp_t flags)
{
	int node = cpu_to_node(cpu);
	struct page *pages;
	struct vmcs *vmcs;

	pages = __alloc_pages_node(node, flags, vmcs_config.order);
	if (!pages)
		return NULL;
	vmcs = page_address(pages);
	memset(vmcs, 0, vmcs_config.size);

	/* KVM supports Enlightened VMCS v1 only */
	if (static_branch_unlikely(&enable_evmcs))
		vmcs->hdr.revision_id = KVM_EVMCS_VERSION;
	else
		vmcs->hdr.revision_id = vmcs_config.revision_id;

	if (shadow)
		vmcs->hdr.shadow_vmcs = 1;
	return vmcs;
}