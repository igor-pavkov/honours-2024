static int close_getadapter_fib(struct aac_dev * dev, void __user *arg)
{
	struct aac_fib_context *fibctx;
	int status;
	unsigned long flags;
	struct list_head * entry;

	/*
	 *	Verify that the HANDLE passed in was a valid AdapterFibContext
	 *
	 *	Search the list of AdapterFibContext addresses on the adapter
	 *	to be sure this is a valid address
	 */

	entry = dev->fib_list.next;
	fibctx = NULL;

	while(entry != &dev->fib_list) {
		fibctx = list_entry(entry, struct aac_fib_context, next);
		/*
		 *	Extract the fibctx from the input parameters
		 */
		if (fibctx->unique == (u32)(uintptr_t)arg) /* We found a winner */
			break;
		entry = entry->next;
		fibctx = NULL;
	}

	if (!fibctx)
		return 0; /* Already gone */

	if((fibctx->type != FSAFS_NTC_GET_ADAPTER_FIB_CONTEXT) ||
		 (fibctx->size != sizeof(struct aac_fib_context)))
		return -EINVAL;
	spin_lock_irqsave(&dev->fib_lock, flags);
	status = aac_close_fib_context(dev, fibctx);
	spin_unlock_irqrestore(&dev->fib_lock, flags);
	return status;
}