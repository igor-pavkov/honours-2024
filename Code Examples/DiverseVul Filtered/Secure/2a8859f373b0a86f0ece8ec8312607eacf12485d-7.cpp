static gpa_t FNAME(gva_to_gpa)(struct kvm_vcpu *vcpu, struct kvm_mmu *mmu,
			       gpa_t addr, u64 access,
			       struct x86_exception *exception)
{
	struct guest_walker walker;
	gpa_t gpa = UNMAPPED_GVA;
	int r;

#ifndef CONFIG_X86_64
	/* A 64-bit GVA should be impossible on 32-bit KVM. */
	WARN_ON_ONCE((addr >> 32) && mmu == vcpu->arch.walk_mmu);
#endif

	r = FNAME(walk_addr_generic)(&walker, vcpu, mmu, addr, access);

	if (r) {
		gpa = gfn_to_gpa(walker.gfn);
		gpa |= addr & ~PAGE_MASK;
	} else if (exception)
		*exception = walker.fault;

	return gpa;
}