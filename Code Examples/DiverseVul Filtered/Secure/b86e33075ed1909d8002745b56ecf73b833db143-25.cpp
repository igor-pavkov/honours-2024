static void __f2fs_submit_merged_bio(struct f2fs_sb_info *sbi,
				struct inode *inode, struct page *page,
				nid_t ino, enum page_type type, int rw)
{
	enum page_type btype = PAGE_TYPE_OF_BIO(type);
	struct f2fs_bio_info *io;

	io = is_read_io(rw) ? &sbi->read_io : &sbi->write_io[btype];

	down_write(&io->io_rwsem);

	if (!__has_merged_page(io, inode, page, ino))
		goto out;

	/* change META to META_FLUSH in the checkpoint procedure */
	if (type >= META_FLUSH) {
		io->fio.type = META_FLUSH;
		io->fio.op = REQ_OP_WRITE;
		io->fio.op_flags = REQ_PREFLUSH | REQ_META | REQ_PRIO;
		if (!test_opt(sbi, NOBARRIER))
			io->fio.op_flags |= REQ_FUA;
	}
	__submit_merged_bio(io);
out:
	up_write(&io->io_rwsem);
}