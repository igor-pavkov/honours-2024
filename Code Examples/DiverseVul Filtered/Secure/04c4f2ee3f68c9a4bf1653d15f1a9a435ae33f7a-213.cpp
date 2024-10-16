static noinstr void vmx_l1d_flush(struct kvm_vcpu *vcpu)
{
	int size = PAGE_SIZE << L1D_CACHE_ORDER;

	/*
	 * This code is only executed when the the flush mode is 'cond' or
	 * 'always'
	 */
	if (static_branch_likely(&vmx_l1d_flush_cond)) {
		bool flush_l1d;

		/*
		 * Clear the per-vcpu flush bit, it gets set again
		 * either from vcpu_run() or from one of the unsafe
		 * VMEXIT handlers.
		 */
		flush_l1d = vcpu->arch.l1tf_flush_l1d;
		vcpu->arch.l1tf_flush_l1d = false;

		/*
		 * Clear the per-cpu flush bit, it gets set again from
		 * the interrupt handlers.
		 */
		flush_l1d |= kvm_get_cpu_l1tf_flush_l1d();
		kvm_clear_cpu_l1tf_flush_l1d();

		if (!flush_l1d)
			return;
	}

	vcpu->stat.l1d_flush++;

	if (static_cpu_has(X86_FEATURE_FLUSH_L1D)) {
		native_wrmsrl(MSR_IA32_FLUSH_CMD, L1D_FLUSH);
		return;
	}

	asm volatile(
		/* First ensure the pages are in the TLB */
		"xorl	%%eax, %%eax\n"
		".Lpopulate_tlb:\n\t"
		"movzbl	(%[flush_pages], %%" _ASM_AX "), %%ecx\n\t"
		"addl	$4096, %%eax\n\t"
		"cmpl	%%eax, %[size]\n\t"
		"jne	.Lpopulate_tlb\n\t"
		"xorl	%%eax, %%eax\n\t"
		"cpuid\n\t"
		/* Now fill the cache */
		"xorl	%%eax, %%eax\n"
		".Lfill_cache:\n"
		"movzbl	(%[flush_pages], %%" _ASM_AX "), %%ecx\n\t"
		"addl	$64, %%eax\n\t"
		"cmpl	%%eax, %[size]\n\t"
		"jne	.Lfill_cache\n\t"
		"lfence\n"
		:: [flush_pages] "r" (vmx_l1d_flush_pages),
		    [size] "r" (size)
		: "eax", "ebx", "ecx", "edx");
}