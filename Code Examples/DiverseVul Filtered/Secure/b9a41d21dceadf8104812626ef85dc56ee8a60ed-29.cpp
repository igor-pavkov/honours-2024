static int __split_and_process_non_flush(struct clone_info *ci)
{
	struct bio *bio = ci->bio;
	struct dm_target *ti;
	unsigned len;
	int r;

	if (unlikely(bio_op(bio) == REQ_OP_DISCARD))
		return __send_discard(ci);
	else if (unlikely(bio_op(bio) == REQ_OP_WRITE_SAME))
		return __send_write_same(ci);
	else if (unlikely(bio_op(bio) == REQ_OP_WRITE_ZEROES))
		return __send_write_zeroes(ci);

	ti = dm_table_find_target(ci->map, ci->sector);
	if (!dm_target_is_valid(ti))
		return -EIO;

	if (bio_op(bio) == REQ_OP_ZONE_REPORT)
		len = ci->sector_count;
	else
		len = min_t(sector_t, max_io_len(ci->sector, ti),
			    ci->sector_count);

	r = __clone_and_map_data_bio(ci, ti, ci->sector, &len);
	if (r < 0)
		return r;

	ci->sector += len;
	ci->sector_count -= len;

	return 0;
}