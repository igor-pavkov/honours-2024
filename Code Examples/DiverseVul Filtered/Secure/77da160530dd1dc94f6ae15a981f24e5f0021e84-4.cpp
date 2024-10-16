static int disk_events_set_dfl_poll_msecs(const char *val,
					  const struct kernel_param *kp)
{
	struct disk_events *ev;
	int ret;

	ret = param_set_ulong(val, kp);
	if (ret < 0)
		return ret;

	mutex_lock(&disk_events_mutex);

	list_for_each_entry(ev, &disk_events, node)
		disk_flush_events(ev->disk, 0);

	mutex_unlock(&disk_events_mutex);

	return 0;
}