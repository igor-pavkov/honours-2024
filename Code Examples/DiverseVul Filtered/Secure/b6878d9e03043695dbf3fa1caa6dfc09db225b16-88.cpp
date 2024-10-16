struct bio *bio_alloc_mddev(gfp_t gfp_mask, int nr_iovecs,
			    struct mddev *mddev)
{
	struct bio *b;

	if (!mddev || !mddev->bio_set)
		return bio_alloc(gfp_mask, nr_iovecs);

	b = bio_alloc_bioset(gfp_mask, nr_iovecs, mddev->bio_set);
	if (!b)
		return NULL;
	return b;
}