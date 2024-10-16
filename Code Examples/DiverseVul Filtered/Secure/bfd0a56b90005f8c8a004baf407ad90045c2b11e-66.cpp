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
	err = -ENOMEM;
	if (!vmx->guest_msrs) {
		goto uninit_vcpu;
	}

	vmx->loaded_vmcs = &vmx->vmcs01;
	vmx->loaded_vmcs->vmcs = alloc_vmcs();
	if (!vmx->loaded_vmcs->vmcs)
		goto free_msrs;
	if (!vmm_exclusive)
		kvm_cpu_vmxon(__pa(per_cpu(vmxarea, raw_smp_processor_id())));
	loaded_vmcs_init(vmx->loaded_vmcs);
	if (!vmm_exclusive)
		kvm_cpu_vmxoff();

	cpu = get_cpu();
	vmx_vcpu_load(&vmx->vcpu, cpu);
	vmx->vcpu.cpu = cpu;
	err = vmx_vcpu_setup(vmx);
	vmx_vcpu_put(&vmx->vcpu);
	put_cpu();
	if (err)
		goto free_vmcs;
	if (vm_need_virtualize_apic_accesses(kvm)) {
		err = alloc_apic_access_page(kvm);
		if (err)
			goto free_vmcs;
	}

	if (enable_ept) {
		if (!kvm->arch.ept_identity_map_addr)
			kvm->arch.ept_identity_map_addr =
				VMX_EPT_IDENTITY_PAGETABLE_ADDR;
		err = -ENOMEM;
		if (alloc_identity_pagetable(kvm) != 0)
			goto free_vmcs;
		if (!init_rmode_identity_map(kvm))
			goto free_vmcs;
	}

	vmx->nested.current_vmptr = -1ull;
	vmx->nested.current_vmcs12 = NULL;

	return &vmx->vcpu;

free_vmcs:
	free_loaded_vmcs(vmx->loaded_vmcs);
free_msrs:
	kfree(vmx->guest_msrs);
uninit_vcpu:
	kvm_vcpu_uninit(&vmx->vcpu);
free_vcpu:
	free_vpid(vmx);
	kmem_cache_free(kvm_vcpu_cache, vmx);
	return ERR_PTR(err);
}