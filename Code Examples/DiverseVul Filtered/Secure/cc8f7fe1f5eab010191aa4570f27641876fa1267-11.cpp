int blk_rq_map_kern(struct request_queue *q, struct request *rq, void *kbuf,
		    unsigned int len, gfp_t gfp_mask)
{
	int reading = rq_data_dir(rq) == READ;
	unsigned long addr = (unsigned long) kbuf;
	struct bio *bio;
	int ret;

	if (len > (queue_max_hw_sectors(q) << 9))
		return -EINVAL;
	if (!len || !kbuf)
		return -EINVAL;

	if (!blk_rq_aligned(q, addr, len) || object_is_on_stack(kbuf) ||
	    blk_queue_may_bounce(q))
		bio = bio_copy_kern(q, kbuf, len, gfp_mask, reading);
	else
		bio = bio_map_kern(q, kbuf, len, gfp_mask);

	if (IS_ERR(bio))
		return PTR_ERR(bio);

	bio->bi_opf &= ~REQ_OP_MASK;
	bio->bi_opf |= req_op(rq);

	ret = blk_rq_append_bio(rq, bio);
	if (unlikely(ret))
		bio_put(bio);
	return ret;
}