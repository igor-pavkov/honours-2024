static void qdisc_class_hash_free(struct hlist_head *h, unsigned int n)
{
	unsigned int size = n * sizeof(struct hlist_head);

	if (size <= PAGE_SIZE)
		kfree(h);
	else
		free_pages((unsigned long)h, get_order(size));
}