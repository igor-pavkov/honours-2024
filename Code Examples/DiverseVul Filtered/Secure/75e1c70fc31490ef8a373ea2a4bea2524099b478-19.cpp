static int __aio_run_iocbs(struct kioctx *ctx)
{
	struct kiocb *iocb;
	struct list_head run_list;

	assert_spin_locked(&ctx->ctx_lock);

	list_replace_init(&ctx->run_list, &run_list);
	while (!list_empty(&run_list)) {
		iocb = list_entry(run_list.next, struct kiocb,
			ki_run_list);
		list_del(&iocb->ki_run_list);
		/*
		 * Hold an extra reference while retrying i/o.
		 */
		iocb->ki_users++;       /* grab extra reference */
		aio_run_iocb(iocb);
		__aio_put_req(ctx, iocb);
 	}
	if (!list_empty(&ctx->run_list))
		return 1;
	return 0;
}