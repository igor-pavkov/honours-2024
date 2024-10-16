static int __floppy_read_block_0(struct block_device *bdev, int drive)
{
	struct bio bio;
	struct bio_vec bio_vec;
	struct page *page;
	struct rb0_cbdata cbdata;

	page = alloc_page(GFP_NOIO);
	if (!page) {
		process_fd_request();
		return -ENOMEM;
	}

	cbdata.drive = drive;

	bio_init(&bio, bdev, &bio_vec, 1, REQ_OP_READ);
	bio_add_page(&bio, page, block_size(bdev), 0);

	bio.bi_iter.bi_sector = 0;
	bio.bi_flags |= (1 << BIO_QUIET);
	bio.bi_private = &cbdata;
	bio.bi_end_io = floppy_rb0_cb;

	init_completion(&cbdata.complete);

	submit_bio(&bio);
	process_fd_request();

	wait_for_completion(&cbdata.complete);

	__free_page(page);

	return 0;
}