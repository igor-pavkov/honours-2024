static void f2fs_read_end_io(struct bio *bio)
{
	struct bio_vec *bvec;
	int i;

#ifdef CONFIG_F2FS_FAULT_INJECTION
	if (time_to_inject(F2FS_P_SB(bio->bi_io_vec->bv_page), FAULT_IO))
		bio->bi_error = -EIO;
#endif

	if (f2fs_bio_encrypted(bio)) {
		if (bio->bi_error) {
			fscrypt_release_ctx(bio->bi_private);
		} else {
			fscrypt_decrypt_bio_pages(bio->bi_private, bio);
			return;
		}
	}

	bio_for_each_segment_all(bvec, bio, i) {
		struct page *page = bvec->bv_page;

		if (!bio->bi_error) {
			if (!PageUptodate(page))
				SetPageUptodate(page);
		} else {
			ClearPageUptodate(page);
			SetPageError(page);
		}
		unlock_page(page);
	}
	bio_put(bio);
}