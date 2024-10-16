static void __ldsem_wake_readers(struct ld_semaphore *sem)
{
	struct ldsem_waiter *waiter, *next;
	struct task_struct *tsk;
	long adjust, count;

	/* Try to grant read locks to all readers on the read wait list.
	 * Note the 'active part' of the count is incremented by
	 * the number of readers before waking any processes up.
	 */
	adjust = sem->wait_readers * (LDSEM_ACTIVE_BIAS - LDSEM_WAIT_BIAS);
	count = ldsem_atomic_update(adjust, sem);
	do {
		if (count > 0)
			break;
		if (ldsem_cmpxchg(&count, count - adjust, sem))
			return;
	} while (1);

	list_for_each_entry_safe(waiter, next, &sem->read_wait, list) {
		tsk = waiter->task;
		smp_mb();
		waiter->task = NULL;
		wake_up_process(tsk);
		put_task_struct(tsk);
	}
	INIT_LIST_HEAD(&sem->read_wait);
	sem->wait_readers = 0;
}