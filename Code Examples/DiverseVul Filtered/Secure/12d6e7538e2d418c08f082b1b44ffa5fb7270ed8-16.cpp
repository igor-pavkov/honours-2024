static void kvm_destroy_dirty_bitmap(struct kvm_memory_slot *memslot)
{
	if (!memslot->dirty_bitmap)
		return;

	kvm_kvfree(memslot->dirty_bitmap);
	memslot->dirty_bitmap = NULL;
}