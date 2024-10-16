int f2fs_submit_page_mbio(struct f2fs_io_info *fio)
{
	struct f2fs_sb_info *sbi = fio->sbi;
	enum page_type btype = PAGE_TYPE_OF_BIO(fio->type);
	struct f2fs_bio_info *io;
	bool is_read = is_read_io(fio->op);
	struct page *bio_page;
	int err = 0;

	io = is_read ? &sbi->read_io : &sbi->write_io[btype];

	if (fio->old_blkaddr != NEW_ADDR)
		verify_block_addr(sbi, fio->old_blkaddr);
	verify_block_addr(sbi, fio->new_blkaddr);

	bio_page = fio->encrypted_page ? fio->encrypted_page : fio->page;

	if (!is_read)
		inc_page_count(sbi, WB_DATA_TYPE(bio_page));

	down_write(&io->io_rwsem);

	if (io->bio && (io->last_block_in_bio != fio->new_blkaddr - 1 ||
	    (io->fio.op != fio->op || io->fio.op_flags != fio->op_flags) ||
			!__same_bdev(sbi, fio->new_blkaddr, io->bio)))
		__submit_merged_bio(io);
alloc_new:
	if (io->bio == NULL) {
		if ((fio->type == DATA || fio->type == NODE) &&
				fio->new_blkaddr & F2FS_IO_SIZE_MASK(sbi)) {
			err = -EAGAIN;
			dec_page_count(sbi, WB_DATA_TYPE(bio_page));
			goto out_fail;
		}
		io->bio = __bio_alloc(sbi, fio->new_blkaddr,
						BIO_MAX_PAGES, is_read);
		io->fio = *fio;
	}

	if (bio_add_page(io->bio, bio_page, PAGE_SIZE, 0) <
							PAGE_SIZE) {
		__submit_merged_bio(io);
		goto alloc_new;
	}

	io->last_block_in_bio = fio->new_blkaddr;
	f2fs_trace_ios(fio, 0);
out_fail:
	up_write(&io->io_rwsem);
	trace_f2fs_submit_page_mbio(fio->page, fio);
	return err;
}