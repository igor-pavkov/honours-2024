static struct dm_table *dm_get_inactive_table(struct mapped_device *md, int *srcu_idx)
{
	struct hash_cell *hc;
	struct dm_table *table = NULL;

	/* increment rcu count, we don't care about the table pointer */
	dm_get_live_table(md, srcu_idx);

	down_read(&_hash_lock);
	hc = dm_get_mdptr(md);
	if (!hc || hc->md != md) {
		DMWARN("device has been removed from the dev hash table.");
		goto out;
	}

	table = hc->new_map;

out:
	up_read(&_hash_lock);

	return table;
}