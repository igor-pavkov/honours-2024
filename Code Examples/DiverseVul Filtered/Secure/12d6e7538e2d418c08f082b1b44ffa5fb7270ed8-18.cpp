void mark_page_dirty_in_slot(struct kvm *kvm, struct kvm_memory_slot *memslot,
			     gfn_t gfn)
{
	if (memslot && memslot->dirty_bitmap) {
		unsigned long rel_gfn = gfn - memslot->base_gfn;

		/* TODO: introduce set_bit_le() and use it */
		test_and_set_bit_le(rel_gfn, memslot->dirty_bitmap);
	}
}