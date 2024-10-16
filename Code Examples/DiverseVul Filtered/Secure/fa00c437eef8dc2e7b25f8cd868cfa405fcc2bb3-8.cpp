static int open_getadapter_fib(struct aac_dev * dev, void __user *arg)
{
	struct aac_fib_context * fibctx;
	int status;

	fibctx = kmalloc(sizeof(struct aac_fib_context), GFP_KERNEL);
	if (fibctx == NULL) {
		status = -ENOMEM;
	} else {
		unsigned long flags;
		struct list_head * entry;
		struct aac_fib_context * context;

		fibctx->type = FSAFS_NTC_GET_ADAPTER_FIB_CONTEXT;
		fibctx->size = sizeof(struct aac_fib_context);
		/*
		 *	Yes yes, I know this could be an index, but we have a
		 * better guarantee of uniqueness for the locked loop below.
		 * Without the aid of a persistent history, this also helps
		 * reduce the chance that the opaque context would be reused.
		 */
		fibctx->unique = (u32)((ulong)fibctx & 0xFFFFFFFF);
		/*
		 *	Initialize the mutex used to wait for the next AIF.
		 */
		sema_init(&fibctx->wait_sem, 0);
		fibctx->wait = 0;
		/*
		 *	Initialize the fibs and set the count of fibs on
		 *	the list to 0.
		 */
		fibctx->count = 0;
		INIT_LIST_HEAD(&fibctx->fib_list);
		fibctx->jiffies = jiffies/HZ;
		/*
		 *	Now add this context onto the adapter's
		 *	AdapterFibContext list.
		 */
		spin_lock_irqsave(&dev->fib_lock, flags);
		/* Ensure that we have a unique identifier */
		entry = dev->fib_list.next;
		while (entry != &dev->fib_list) {
			context = list_entry(entry, struct aac_fib_context, next);
			if (context->unique == fibctx->unique) {
				/* Not unique (32 bits) */
				fibctx->unique++;
				entry = dev->fib_list.next;
			} else {
				entry = entry->next;
			}
		}
		list_add_tail(&fibctx->next, &dev->fib_list);
		spin_unlock_irqrestore(&dev->fib_lock, flags);
		if (copy_to_user(arg, &fibctx->unique,
						sizeof(fibctx->unique))) {
			status = -EFAULT;
		} else {
			status = 0;
		}
	}
	return status;
}