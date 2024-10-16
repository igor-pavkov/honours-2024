void dm_accept_partial_bio(struct bio *bio, unsigned n_sectors)
{
	struct dm_target_io *tio = container_of(bio, struct dm_target_io, clone);
	unsigned bi_size = bio->bi_iter.bi_size >> SECTOR_SHIFT;
	BUG_ON(bio->bi_opf & REQ_PREFLUSH);
	BUG_ON(bi_size > *tio->len_ptr);
	BUG_ON(n_sectors > bi_size);
	*tio->len_ptr -= bi_size - n_sectors;
	bio->bi_iter.bi_size = n_sectors << SECTOR_SHIFT;
}