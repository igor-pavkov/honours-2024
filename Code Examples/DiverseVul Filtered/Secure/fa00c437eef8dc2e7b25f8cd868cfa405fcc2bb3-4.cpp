static int next_getadapter_fib(struct aac_dev * dev, void __user *arg)
{
	struct fib_ioctl f;
	struct fib *fib;
	struct aac_fib_context *fibctx;
	int status;
	struct list_head * entry;
	unsigned long flags;

	if(copy_from_user((void *)&f, arg, sizeof(struct fib_ioctl)))
		return -EFAULT;
	/*
	 *	Verify that the HANDLE passed in was a valid AdapterFibContext
	 *
	 *	Search the list of AdapterFibContext addresses on the adapter
	 *	to be sure this is a valid address
	 */
	spin_lock_irqsave(&dev->fib_lock, flags);
	entry = dev->fib_list.next;
	fibctx = NULL;

	while (entry != &dev->fib_list) {
		fibctx = list_entry(entry, struct aac_fib_context, next);
		/*
		 *	Extract the AdapterFibContext from the Input parameters.
		 */
		if (fibctx->unique == f.fibctx) { /* We found a winner */
			break;
		}
		entry = entry->next;
		fibctx = NULL;
	}
	if (!fibctx) {
		spin_unlock_irqrestore(&dev->fib_lock, flags);
		dprintk ((KERN_INFO "Fib Context not found\n"));
		return -EINVAL;
	}

	if((fibctx->type != FSAFS_NTC_GET_ADAPTER_FIB_CONTEXT) ||
		 (fibctx->size != sizeof(struct aac_fib_context))) {
		spin_unlock_irqrestore(&dev->fib_lock, flags);
		dprintk ((KERN_INFO "Fib Context corrupt?\n"));
		return -EINVAL;
	}
	status = 0;
	/*
	 *	If there are no fibs to send back, then either wait or return
	 *	-EAGAIN
	 */
return_fib:
	if (!list_empty(&fibctx->fib_list)) {
		/*
		 *	Pull the next fib from the fibs
		 */
		entry = fibctx->fib_list.next;
		list_del(entry);

		fib = list_entry(entry, struct fib, fiblink);
		fibctx->count--;
		spin_unlock_irqrestore(&dev->fib_lock, flags);
		if (copy_to_user(f.fib, fib->hw_fib_va, sizeof(struct hw_fib))) {
			kfree(fib->hw_fib_va);
			kfree(fib);
			return -EFAULT;
		}
		/*
		 *	Free the space occupied by this copy of the fib.
		 */
		kfree(fib->hw_fib_va);
		kfree(fib);
		status = 0;
	} else {
		spin_unlock_irqrestore(&dev->fib_lock, flags);
		/* If someone killed the AIF aacraid thread, restart it */
		status = !dev->aif_thread;
		if (status && !dev->in_reset && dev->queues && dev->fsa_dev) {
			/* Be paranoid, be very paranoid! */
			kthread_stop(dev->thread);
			ssleep(1);
			dev->aif_thread = 0;
			dev->thread = kthread_run(aac_command_thread, dev,
						  "%s", dev->name);
			ssleep(1);
		}
		if (f.wait) {
			if(down_interruptible(&fibctx->wait_sem) < 0) {
				status = -ERESTARTSYS;
			} else {
				/* Lock again and retry */
				spin_lock_irqsave(&dev->fib_lock, flags);
				goto return_fib;
			}
		} else {
			status = -EAGAIN;
		}
	}
	fibctx->jiffies = jiffies/HZ;
	return status;
}