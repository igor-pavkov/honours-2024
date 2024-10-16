int aac_close_fib_context(struct aac_dev * dev, struct aac_fib_context * fibctx)
{
	struct fib *fib;

	/*
	 *	First free any FIBs that have not been consumed.
	 */
	while (!list_empty(&fibctx->fib_list)) {
		struct list_head * entry;
		/*
		 *	Pull the next fib from the fibs
		 */
		entry = fibctx->fib_list.next;
		list_del(entry);
		fib = list_entry(entry, struct fib, fiblink);
		fibctx->count--;
		/*
		 *	Free the space occupied by this copy of the fib.
		 */
		kfree(fib->hw_fib_va);
		kfree(fib);
	}
	/*
	 *	Remove the Context from the AdapterFibContext List
	 */
	list_del(&fibctx->next);
	/*
	 *	Invalidate context
	 */
	fibctx->type = 0;
	/*
	 *	Free the space occupied by the Context
	 */
	kfree(fibctx);
	return 0;
}