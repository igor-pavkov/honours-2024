void rds_atomic_free_op(struct rm_atomic_op *ao)
{
	struct page *page = sg_page(ao->op_sg);

	/* Mark page dirty if it was possibly modified, which
	 * is the case for a RDMA_READ which copies from remote
	 * to local memory */
	set_page_dirty(page);
	put_page(page);

	kfree(ao->op_notifier);
	ao->op_notifier = NULL;
	ao->op_active = 0;
}