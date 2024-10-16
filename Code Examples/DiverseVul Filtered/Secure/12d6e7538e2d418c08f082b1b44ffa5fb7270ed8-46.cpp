static int kvm_create_dirty_bitmap(struct kvm_memory_slot *memslot)
{
#ifndef CONFIG_S390
	unsigned long dirty_bytes = 2 * kvm_dirty_bitmap_bytes(memslot);

	memslot->dirty_bitmap = kvm_kvzalloc(dirty_bytes);
	if (!memslot->dirty_bitmap)
		return -ENOMEM;

#endif /* !CONFIG_S390 */
	return 0;
}