static unsigned long gfn_to_hva_read(struct kvm *kvm, gfn_t gfn)
{
	return __gfn_to_hva_many(gfn_to_memslot(kvm, gfn), gfn, NULL, false);
}