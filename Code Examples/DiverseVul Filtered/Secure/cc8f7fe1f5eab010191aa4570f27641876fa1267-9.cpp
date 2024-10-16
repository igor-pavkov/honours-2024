int blk_rq_unmap_user(struct bio *bio)
{
	struct bio *next_bio;
	int ret = 0, ret2;

	while (bio) {
		if (bio->bi_private) {
			ret2 = bio_uncopy_user(bio);
			if (ret2 && !ret)
				ret = ret2;
		} else {
			bio_release_pages(bio, bio_data_dir(bio) == READ);
		}

		next_bio = bio;
		bio = bio->bi_next;
		bio_put(next_bio);
	}

	return ret;
}