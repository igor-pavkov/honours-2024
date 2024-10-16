static void bio_setup_sector(struct bio *bio, sector_t sector, unsigned len)
{
	bio->bi_iter.bi_sector = sector;
	bio->bi_iter.bi_size = to_bytes(len);
}