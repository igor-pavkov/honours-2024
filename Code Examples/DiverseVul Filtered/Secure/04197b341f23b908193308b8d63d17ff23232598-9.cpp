xfs_destroy_ioend(
	struct xfs_ioend	*ioend,
	int			error)
{
	struct inode		*inode = ioend->io_inode;
	struct bio		*last = ioend->io_bio;
	struct bio		*bio, *next;

	for (bio = &ioend->io_inline_bio; bio; bio = next) {
		struct bio_vec	*bvec;
		int		i;

		/*
		 * For the last bio, bi_private points to the ioend, so we
		 * need to explicitly end the iteration here.
		 */
		if (bio == last)
			next = NULL;
		else
			next = bio->bi_private;

		/* walk each page on bio, ending page IO on them */
		bio_for_each_segment_all(bvec, bio, i)
			xfs_finish_page_writeback(inode, bvec, error);

		bio_put(bio);
	}
}