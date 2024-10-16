static int bio_map_user_iov(struct request *rq, struct iov_iter *iter,
		gfp_t gfp_mask)
{
	unsigned int max_sectors = queue_max_hw_sectors(rq->q);
	struct bio *bio;
	int ret;
	int j;

	if (!iov_iter_count(iter))
		return -EINVAL;

	bio = bio_kmalloc(gfp_mask, iov_iter_npages(iter, BIO_MAX_VECS));
	if (!bio)
		return -ENOMEM;
	bio->bi_opf |= req_op(rq);

	while (iov_iter_count(iter)) {
		struct page **pages;
		ssize_t bytes;
		size_t offs, added = 0;
		int npages;

		bytes = iov_iter_get_pages_alloc(iter, &pages, LONG_MAX, &offs);
		if (unlikely(bytes <= 0)) {
			ret = bytes ? bytes : -EFAULT;
			goto out_unmap;
		}

		npages = DIV_ROUND_UP(offs + bytes, PAGE_SIZE);

		if (unlikely(offs & queue_dma_alignment(rq->q))) {
			ret = -EINVAL;
			j = 0;
		} else {
			for (j = 0; j < npages; j++) {
				struct page *page = pages[j];
				unsigned int n = PAGE_SIZE - offs;
				bool same_page = false;

				if (n > bytes)
					n = bytes;

				if (!bio_add_hw_page(rq->q, bio, page, n, offs,
						     max_sectors, &same_page)) {
					if (same_page)
						put_page(page);
					break;
				}

				added += n;
				bytes -= n;
				offs = 0;
			}
			iov_iter_advance(iter, added);
		}
		/*
		 * release the pages we didn't map into the bio, if any
		 */
		while (j < npages)
			put_page(pages[j++]);
		kvfree(pages);
		/* couldn't stuff something into bio? */
		if (bytes)
			break;
	}

	ret = blk_rq_append_bio(rq, bio);
	if (ret)
		goto out_unmap;
	return 0;

 out_unmap:
	bio_release_pages(bio, false);
	bio_put(bio);
	return ret;
}