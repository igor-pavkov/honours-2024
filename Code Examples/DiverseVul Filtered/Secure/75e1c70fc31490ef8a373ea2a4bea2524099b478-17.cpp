static void aio_batch_free(struct hlist_head *batch_hash)
{
	struct aio_batch_entry *abe;
	struct hlist_node *pos, *n;
	int i;

	for (i = 0; i < AIO_BATCH_HASH_SIZE; i++) {
		hlist_for_each_entry_safe(abe, pos, n, &batch_hash[i], list) {
			blk_run_address_space(abe->mapping);
			iput(abe->mapping->host);
			hlist_del(&abe->list);
			mempool_free(abe, abe_pool);
		}
	}
}