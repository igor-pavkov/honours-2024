static bool reexecute_instruction(struct kvm_vcpu *vcpu, gva_t cr2,
				  bool write_fault_to_shadow_pgtable)
{
	gpa_t gpa = cr2;
	pfn_t pfn;

	if (!vcpu->arch.mmu.direct_map) {
		/*
		 * Write permission should be allowed since only
		 * write access need to be emulated.
		 */
		gpa = kvm_mmu_gva_to_gpa_write(vcpu, cr2, NULL);

		/*
		 * If the mapping is invalid in guest, let cpu retry
		 * it to generate fault.
		 */
		if (gpa == UNMAPPED_GVA)
			return true;
	}

	/*
	 * Do not retry the unhandleable instruction if it faults on the
	 * readonly host memory, otherwise it will goto a infinite loop:
	 * retry instruction -> write #PF -> emulation fail -> retry
	 * instruction -> ...
	 */
	pfn = gfn_to_pfn(vcpu->kvm, gpa_to_gfn(gpa));

	/*
	 * If the instruction failed on the error pfn, it can not be fixed,
	 * report the error to userspace.
	 */
	if (is_error_noslot_pfn(pfn))
		return false;

	kvm_release_pfn_clean(pfn);

	/* The instructions are well-emulated on direct mmu. */
	if (vcpu->arch.mmu.direct_map) {
		unsigned int indirect_shadow_pages;

		spin_lock(&vcpu->kvm->mmu_lock);
		indirect_shadow_pages = vcpu->kvm->arch.indirect_shadow_pages;
		spin_unlock(&vcpu->kvm->mmu_lock);

		if (indirect_shadow_pages)
			kvm_mmu_unprotect_page(vcpu->kvm, gpa_to_gfn(gpa));

		return true;
	}

	/*
	 * if emulation was due to access to shadowed page table
	 * and it failed try to unshadow page and re-enter the
	 * guest to let CPU execute the instruction.
	 */
	kvm_mmu_unprotect_page(vcpu->kvm, gpa_to_gfn(gpa));

	/*
	 * If the access faults on its page table, it can not
	 * be fixed by unprotecting shadow page and it should
	 * be reported to userspace.
	 */
	return !write_fault_to_shadow_pgtable;
}