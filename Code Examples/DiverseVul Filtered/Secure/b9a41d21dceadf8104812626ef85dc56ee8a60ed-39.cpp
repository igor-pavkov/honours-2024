static void __bind_mempools(struct mapped_device *md, struct dm_table *t)
{
	struct dm_md_mempools *p = dm_table_get_md_mempools(t);

	if (md->bs) {
		/* The md already has necessary mempools. */
		if (dm_table_bio_based(t)) {
			/*
			 * Reload bioset because front_pad may have changed
			 * because a different table was loaded.
			 */
			bioset_free(md->bs);
			md->bs = p->bs;
			p->bs = NULL;
		}
		/*
		 * There's no need to reload with request-based dm
		 * because the size of front_pad doesn't change.
		 * Note for future: If you are to reload bioset,
		 * prep-ed requests in the queue may refer
		 * to bio from the old bioset, so you must walk
		 * through the queue to unprep.
		 */
		goto out;
	}

	BUG_ON(!p || md->io_pool || md->bs);

	md->io_pool = p->io_pool;
	p->io_pool = NULL;
	md->bs = p->bs;
	p->bs = NULL;

out:
	/* mempool bind completed, no longer need any mempools in the table */
	dm_table_free_md_mempools(t);
}