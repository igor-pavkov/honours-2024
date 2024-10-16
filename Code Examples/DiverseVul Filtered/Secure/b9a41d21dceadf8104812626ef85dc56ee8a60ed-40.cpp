void dm_free_md_mempools(struct dm_md_mempools *pools)
{
	if (!pools)
		return;

	mempool_destroy(pools->io_pool);

	if (pools->bs)
		bioset_free(pools->bs);

	kfree(pools);
}