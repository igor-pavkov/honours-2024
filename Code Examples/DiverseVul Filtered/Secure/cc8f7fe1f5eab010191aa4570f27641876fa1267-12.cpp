static void bio_copy_kern_endio_read(struct bio *bio)
{
	char *p = bio->bi_private;
	struct bio_vec *bvec;
	struct bvec_iter_all iter_all;

	bio_for_each_segment_all(bvec, bio, iter_all) {
		memcpy_from_bvec(p, bvec);
		p += bvec->bv_len;
	}

	bio_copy_kern_endio(bio);
}