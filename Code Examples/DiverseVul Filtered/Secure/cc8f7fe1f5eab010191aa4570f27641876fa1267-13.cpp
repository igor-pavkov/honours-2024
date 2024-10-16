int blk_rq_append_bio(struct request *rq, struct bio *bio)
{
	struct bvec_iter iter;
	struct bio_vec bv;
	unsigned int nr_segs = 0;

	bio_for_each_bvec(bv, bio, iter)
		nr_segs++;

	if (!rq->bio) {
		blk_rq_bio_prep(rq, bio, nr_segs);
	} else {
		if (!ll_back_merge_fn(rq, bio, nr_segs))
			return -EINVAL;
		rq->biotail->bi_next = bio;
		rq->biotail = bio;
		rq->__data_len += (bio)->bi_iter.bi_size;
		bio_crypt_free_ctx(bio);
	}

	return 0;
}