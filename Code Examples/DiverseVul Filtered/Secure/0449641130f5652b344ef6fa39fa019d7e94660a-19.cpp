static int ext4_run_lazyinit_thread(void)
{
	ext4_lazyinit_task = kthread_run(ext4_lazyinit_thread,
					 ext4_li_info, "ext4lazyinit");
	if (IS_ERR(ext4_lazyinit_task)) {
		int err = PTR_ERR(ext4_lazyinit_task);
		ext4_clear_request_list();
		del_timer_sync(&ext4_li_info->li_timer);
		kfree(ext4_li_info);
		ext4_li_info = NULL;
		printk(KERN_CRIT "EXT4: error %d creating inode table "
				 "initialization thread\n",
				 err);
		return err;
	}
	ext4_li_info->li_state |= EXT4_LAZYINIT_RUNNING;

	wait_event(ext4_li_info->li_wait_task, ext4_li_info->li_task != NULL);
	return 0;
}