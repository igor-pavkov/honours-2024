static gva_t rmode_tss_base(struct kvm *kvm)
{
	if (!kvm->arch.tss_addr) {
		gfn_t base_gfn = kvm->memslots[0].base_gfn +
				 kvm->memslots[0].npages - 3;
		return base_gfn << PAGE_SHIFT;
	}
	return kvm->arch.tss_addr;
}