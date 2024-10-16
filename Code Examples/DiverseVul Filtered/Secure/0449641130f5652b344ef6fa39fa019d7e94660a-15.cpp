static int ext4_lazyinit_thread(void *arg)
{
	struct ext4_lazy_init *eli = (struct ext4_lazy_init *)arg;
	struct list_head *pos, *n;
	struct ext4_li_request *elr;
	unsigned long next_wakeup;
	DEFINE_WAIT(wait);

	BUG_ON(NULL == eli);

	eli->li_timer.data = (unsigned long)current;
	eli->li_timer.function = ext4_lazyinode_timeout;

	eli->li_task = current;
	wake_up(&eli->li_wait_task);

cont_thread:
	while (true) {
		next_wakeup = MAX_JIFFY_OFFSET;

		mutex_lock(&eli->li_list_mtx);
		if (list_empty(&eli->li_request_list)) {
			mutex_unlock(&eli->li_list_mtx);
			goto exit_thread;
		}

		list_for_each_safe(pos, n, &eli->li_request_list) {
			elr = list_entry(pos, struct ext4_li_request,
					 lr_request);

			if (time_after_eq(jiffies, elr->lr_next_sched)) {
				if (ext4_run_li_request(elr) != 0) {
					/* error, remove the lazy_init job */
					ext4_remove_li_request(elr);
					continue;
				}
			}

			if (time_before(elr->lr_next_sched, next_wakeup))
				next_wakeup = elr->lr_next_sched;
		}
		mutex_unlock(&eli->li_list_mtx);

		if (freezing(current))
			refrigerator();

		if ((time_after_eq(jiffies, next_wakeup)) ||
		    (MAX_JIFFY_OFFSET == next_wakeup)) {
			cond_resched();
			continue;
		}

		eli->li_timer.expires = next_wakeup;
		add_timer(&eli->li_timer);
		prepare_to_wait(&eli->li_wait_daemon, &wait,
				TASK_INTERRUPTIBLE);
		if (time_before(jiffies, next_wakeup))
			schedule();
		finish_wait(&eli->li_wait_daemon, &wait);
		if (kthread_should_stop()) {
			ext4_clear_request_list();
			goto exit_thread;
		}
	}

exit_thread:
	/*
	 * It looks like the request list is empty, but we need
	 * to check it under the li_list_mtx lock, to prevent any
	 * additions into it, and of course we should lock ext4_li_mtx
	 * to atomically free the list and ext4_li_info, because at
	 * this point another ext4 filesystem could be registering
	 * new one.
	 */
	mutex_lock(&ext4_li_mtx);
	mutex_lock(&eli->li_list_mtx);
	if (!list_empty(&eli->li_request_list)) {
		mutex_unlock(&eli->li_list_mtx);
		mutex_unlock(&ext4_li_mtx);
		goto cont_thread;
	}
	mutex_unlock(&eli->li_list_mtx);
	del_timer_sync(&ext4_li_info->li_timer);
	eli->li_task = NULL;
	wake_up(&eli->li_wait_task);

	kfree(ext4_li_info);
	ext4_lazyinit_task = NULL;
	ext4_li_info = NULL;
	mutex_unlock(&ext4_li_mtx);

	return 0;
}