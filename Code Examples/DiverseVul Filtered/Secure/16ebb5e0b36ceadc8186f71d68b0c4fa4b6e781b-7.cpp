static struct hlist_head *qdisc_class_hash_alloc(unsigned int n)
{
	unsigned int size = n * sizeof(struct hlist_head), i;
	struct hlist_head *h;

	if (size <= PAGE_SIZE)
		h = kmalloc(size, GFP_KERNEL);
	else
		h = (struct hlist_head *)
			__get_free_pages(GFP_KERNEL, get_order(size));

	if (h != NULL) {
		for (i = 0; i < n; i++)
			INIT_HLIST_HEAD(&h[i]);
	}
	return h;
}