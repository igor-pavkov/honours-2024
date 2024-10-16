static bool has_merged_page(struct f2fs_sb_info *sbi, struct inode *inode,
						struct page *page, nid_t ino,
						enum page_type type)
{
	enum page_type btype = PAGE_TYPE_OF_BIO(type);
	struct f2fs_bio_info *io = &sbi->write_io[btype];
	bool ret;

	down_read(&io->io_rwsem);
	ret = __has_merged_page(io, inode, page, ino);
	up_read(&io->io_rwsem);
	return ret;
}