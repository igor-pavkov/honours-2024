static int snd_timer_dev_disconnect(struct snd_device *device)
{
	struct snd_timer *timer = device->device_data;
	struct snd_timer_instance *ti;

	mutex_lock(&register_mutex);
	list_del_init(&timer->device_list);
	/* wake up pending sleepers */
	list_for_each_entry(ti, &timer->open_list_head, open_list) {
		if (ti->disconnect)
			ti->disconnect(ti);
	}
	mutex_unlock(&register_mutex);
	return 0;
}