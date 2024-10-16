static int dm_any_congested(void *congested_data, int bdi_bits)
{
	int r = bdi_bits;
	struct mapped_device *md = congested_data;
	struct dm_table *map;

	if (!test_bit(DMF_BLOCK_IO_FOR_SUSPEND, &md->flags)) {
		if (dm_request_based(md)) {
			/*
			 * With request-based DM we only need to check the
			 * top-level queue for congestion.
			 */
			r = md->queue->backing_dev_info->wb.state & bdi_bits;
		} else {
			map = dm_get_live_table_fast(md);
			if (map)
				r = dm_table_any_congested(map, bdi_bits);
			dm_put_live_table_fast(md);
		}
	}

	return r;
}