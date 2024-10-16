int blk_rq_map_user_iov(struct request_queue *q, struct request *rq,
			struct rq_map_data *map_data,
			const struct iov_iter *iter, gfp_t gfp_mask)
{
	bool copy = false;
	unsigned long align = q->dma_pad_mask | queue_dma_alignment(q);
	struct bio *bio = NULL;
	struct iov_iter i;
	int ret;

	if (map_data)
		copy = true;
	else if (iov_iter_alignment(iter) & align)
		copy = true;
	else if (queue_virt_boundary(q))
		copy = queue_virt_boundary(q) & iov_iter_gap_alignment(iter);

	i = *iter;
	do {
		ret =__blk_rq_map_user_iov(rq, map_data, &i, gfp_mask, copy);
		if (ret)
			goto unmap_rq;
		if (!bio)
			bio = rq->bio;
	} while (iov_iter_count(&i));

	if (!bio_flagged(bio, BIO_USER_MAPPED))
		rq->cmd_flags |= REQ_COPY_USER;
	return 0;

unmap_rq:
	__blk_rq_unmap_user(bio);
	rq->bio = NULL;
	return -EINVAL;
}