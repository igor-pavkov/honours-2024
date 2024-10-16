static void aio_batch_add(struct address_space *mapping,
			  struct hlist_head *batch_hash)
{
	struct aio_batch_entry *abe;
	struct hlist_node *pos;
	unsigned bucket;

	bucket = hash_ptr(mapping, AIO_BATCH_HASH_BITS);
	hlist_for_each_entry(abe, pos, &batch_hash[bucket], list) {
		if (abe->mapping == mapping)
			return;
	}

	abe = mempool_alloc(abe_pool, GFP_KERNEL);
	BUG_ON(!igrab(mapping->host));
	abe->mapping = mapping;
	hlist_add_head(&abe->list, &batch_hash[bucket]);
	return;
}