static struct bio *f2fs_grab_bio(struct inode *inode, block_t blkaddr,
				 unsigned nr_pages)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	struct fscrypt_ctx *ctx = NULL;
	struct bio *bio;

	if (f2fs_encrypted_inode(inode) && S_ISREG(inode->i_mode)) {
		ctx = fscrypt_get_ctx(inode, GFP_NOFS);
		if (IS_ERR(ctx))
			return ERR_CAST(ctx);

		/* wait the page to be moved by cleaning */
		f2fs_wait_on_encrypted_page_writeback(sbi, blkaddr);
	}

	bio = bio_alloc(GFP_KERNEL, min_t(int, nr_pages, BIO_MAX_PAGES));
	if (!bio) {
		if (ctx)
			fscrypt_release_ctx(ctx);
		return ERR_PTR(-ENOMEM);
	}
	f2fs_target_device(sbi, blkaddr, bio);
	bio->bi_end_io = f2fs_read_end_io;
	bio->bi_private = ctx;

	return bio;
}