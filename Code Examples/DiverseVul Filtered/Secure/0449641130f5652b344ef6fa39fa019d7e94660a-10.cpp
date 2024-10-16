static int ext4_li_info_new(void)
{
	struct ext4_lazy_init *eli = NULL;

	eli = kzalloc(sizeof(*eli), GFP_KERNEL);
	if (!eli)
		return -ENOMEM;

	eli->li_task = NULL;
	INIT_LIST_HEAD(&eli->li_request_list);
	mutex_init(&eli->li_list_mtx);

	init_waitqueue_head(&eli->li_wait_daemon);
	init_waitqueue_head(&eli->li_wait_task);
	init_timer(&eli->li_timer);
	eli->li_state |= EXT4_LAZYINIT_QUIT;

	ext4_li_info = eli;

	return 0;
}