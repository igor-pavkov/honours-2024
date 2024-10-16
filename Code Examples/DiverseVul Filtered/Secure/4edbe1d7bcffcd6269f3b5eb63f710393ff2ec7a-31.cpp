static void dm_hash_remove_all(bool keep_open_devices, bool mark_deferred, bool only_deferred)
{
	int i, dev_skipped;
	struct hash_cell *hc;
	struct mapped_device *md;
	struct dm_table *t;

retry:
	dev_skipped = 0;

	down_write(&_hash_lock);

	for (i = 0; i < NUM_BUCKETS; i++) {
		list_for_each_entry(hc, _name_buckets + i, name_list) {
			md = hc->md;
			dm_get(md);

			if (keep_open_devices &&
			    dm_lock_for_deletion(md, mark_deferred, only_deferred)) {
				dm_put(md);
				dev_skipped++;
				continue;
			}

			t = __hash_remove(hc);

			up_write(&_hash_lock);

			if (t) {
				dm_sync_table(md);
				dm_table_destroy(t);
			}
			dm_put(md);
			if (likely(keep_open_devices))
				dm_destroy(md);
			else
				dm_destroy_immediate(md);

			/*
			 * Some mapped devices may be using other mapped
			 * devices, so repeat until we make no further
			 * progress.  If a new mapped device is created
			 * here it will also get removed.
			 */
			goto retry;
		}
	}

	up_write(&_hash_lock);

	if (dev_skipped)
		DMWARN("remove_all left %d open device(s)", dev_skipped);
}