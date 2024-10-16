static struct kvm_vcpu *vmx_create_vcpu(struct kvm *kvm, unsigned int id)
{
	int err;
	struct vcpu_vmx *vmx = kmem_cache_zalloc(kvm_vcpu_cache, GFP_KERNEL);
	int cpu;

	if (!vmx)
		return ERR_PTR(-ENOMEM);

	allocate_vpid(vmx);

	err = kvm_vcpu_init(&vmx->vcpu, kvm, id);
	if (err)
		goto free_vcpu;

	vmx->guest_msrs = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (!vmx->guest_msrs) {
		err = -ENOMEM;
		goto uninit_vcpu;
	}

	vmx->host_msrs = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (!vmx->host_msrs)
		goto free_guest_msrs;

	vmx->vmcs = alloc_vmcs();
	if (!vmx->vmcs)
		goto free_msrs;

	vmcs_clear(vmx->vmcs);

	cpu = get_cpu();
	vmx_vcpu_load(&vmx->vcpu, cpu);
	err = vmx_vcpu_setup(vmx);
	vmx_vcpu_put(&vmx->vcpu);
	put_cpu();
	if (err)
		goto free_vmcs;
	if (vm_need_virtualize_apic_accesses(kvm))
		if (alloc_apic_access_page(kvm) != 0)
			goto free_vmcs;

	if (vm_need_ept())
		if (alloc_identity_pagetable(kvm) != 0)
			goto free_vmcs;

	return &vmx->vcpu;

free_vmcs:
	free_vmcs(vmx->vmcs);
free_msrs:
	kfree(vmx->host_msrs);
free_guest_msrs:
	kfree(vmx->guest_msrs);
uninit_vcpu:
	kvm_vcpu_uninit(&vmx->vcpu);
free_vcpu:
	kmem_cache_free(kvm_vcpu_cache, vmx);
	return ERR_PTR(err);
}