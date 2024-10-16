static int __init vmx_init(void)
{
	void *va;
	int r;

	vmx_io_bitmap_a = alloc_page(GFP_KERNEL | __GFP_HIGHMEM);
	if (!vmx_io_bitmap_a)
		return -ENOMEM;

	vmx_io_bitmap_b = alloc_page(GFP_KERNEL | __GFP_HIGHMEM);
	if (!vmx_io_bitmap_b) {
		r = -ENOMEM;
		goto out;
	}

	vmx_msr_bitmap = alloc_page(GFP_KERNEL | __GFP_HIGHMEM);
	if (!vmx_msr_bitmap) {
		r = -ENOMEM;
		goto out1;
	}

	/*
	 * Allow direct access to the PC debug port (it is often used for I/O
	 * delays, but the vmexits simply slow things down).
	 */
	va = kmap(vmx_io_bitmap_a);
	memset(va, 0xff, PAGE_SIZE);
	clear_bit(0x80, va);
	kunmap(vmx_io_bitmap_a);

	va = kmap(vmx_io_bitmap_b);
	memset(va, 0xff, PAGE_SIZE);
	kunmap(vmx_io_bitmap_b);

	va = kmap(vmx_msr_bitmap);
	memset(va, 0xff, PAGE_SIZE);
	kunmap(vmx_msr_bitmap);

	set_bit(0, vmx_vpid_bitmap); /* 0 is reserved for host */

	r = kvm_init(&vmx_x86_ops, sizeof(struct vcpu_vmx), THIS_MODULE);
	if (r)
		goto out2;

	vmx_disable_intercept_for_msr(vmx_msr_bitmap, MSR_FS_BASE);
	vmx_disable_intercept_for_msr(vmx_msr_bitmap, MSR_GS_BASE);
	vmx_disable_intercept_for_msr(vmx_msr_bitmap, MSR_IA32_SYSENTER_CS);
	vmx_disable_intercept_for_msr(vmx_msr_bitmap, MSR_IA32_SYSENTER_ESP);
	vmx_disable_intercept_for_msr(vmx_msr_bitmap, MSR_IA32_SYSENTER_EIP);

	if (vm_need_ept()) {
		bypass_guest_pf = 0;
		kvm_mmu_set_base_ptes(VMX_EPT_READABLE_MASK |
			VMX_EPT_WRITABLE_MASK);
		kvm_mmu_set_mask_ptes(0ull, 0ull, 0ull, 0ull,
				VMX_EPT_EXECUTABLE_MASK,
				VMX_EPT_DEFAULT_MT << VMX_EPT_MT_EPTE_SHIFT);
		kvm_enable_tdp();
	} else
		kvm_disable_tdp();

	if (bypass_guest_pf)
		kvm_mmu_set_nonpresent_ptes(~0xffeull, 0ull);

	ept_sync_global();

	return 0;

out2:
	__free_page(vmx_msr_bitmap);
out1:
	__free_page(vmx_io_bitmap_b);
out:
	__free_page(vmx_io_bitmap_a);
	return r;
}