static int bio_copy_user_iov(struct request *rq, struct rq_map_data *map_data,
		struct iov_iter *iter, gfp_t gfp_mask)
{
	struct bio_map_data *bmd;
	struct page *page;
	struct bio *bio;
	int i = 0, ret;
	int nr_pages;
	unsigned int len = iter->count;
	unsigned int offset = map_data ? offset_in_page(map_data->offset) : 0;

	bmd = bio_alloc_map_data(iter, gfp_mask);
	if (!bmd)
		return -ENOMEM;

	/*
	 * We need to do a deep copy of the iov_iter including the iovecs.
	 * The caller provided iov might point to an on-stack or otherwise
	 * shortlived one.
	 */
	bmd->is_our_pages = !map_data;
	bmd->is_null_mapped = (map_data && map_data->null_mapped);

	nr_pages = bio_max_segs(DIV_ROUND_UP(offset + len, PAGE_SIZE));

	ret = -ENOMEM;
	bio = bio_kmalloc(gfp_mask, nr_pages);
	if (!bio)
		goto out_bmd;
	bio->bi_opf |= req_op(rq);

	if (map_data) {
		nr_pages = 1 << map_data->page_order;
		i = map_data->offset / PAGE_SIZE;
	}
	while (len) {
		unsigned int bytes = PAGE_SIZE;

		bytes -= offset;

		if (bytes > len)
			bytes = len;

		if (map_data) {
			if (i == map_data->nr_entries * nr_pages) {
				ret = -ENOMEM;
				goto cleanup;
			}

			page = map_data->pages[i / nr_pages];
			page += (i % nr_pages);

			i++;
		} else {
			page = alloc_page(GFP_NOIO | gfp_mask);
			if (!page) {
				ret = -ENOMEM;
				goto cleanup;
			}
		}

		if (bio_add_pc_page(rq->q, bio, page, bytes, offset) < bytes) {
			if (!map_data)
				__free_page(page);
			break;
		}

		len -= bytes;
		offset = 0;
	}

	if (map_data)
		map_data->offset += bio->bi_iter.bi_size;

	/*
	 * success
	 */
	if ((iov_iter_rw(iter) == WRITE &&
	     (!map_data || !map_data->null_mapped)) ||
	    (map_data && map_data->from_user)) {
		ret = bio_copy_from_iter(bio, iter);
		if (ret)
			goto cleanup;
	} else {
		if (bmd->is_our_pages)
			zero_fill_bio(bio);
		iov_iter_advance(iter, bio->bi_iter.bi_size);
	}

	bio->bi_private = bmd;

	ret = blk_rq_append_bio(rq, bio);
	if (ret)
		goto cleanup;
	return 0;
cleanup:
	if (!map_data)
		bio_free_pages(bio);
	bio_put(bio);
out_bmd:
	kfree(bmd);
	return ret;
}