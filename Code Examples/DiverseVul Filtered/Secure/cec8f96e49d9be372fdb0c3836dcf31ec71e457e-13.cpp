int snd_timer_close(struct snd_timer_instance *timeri)
{
	struct snd_timer *timer = NULL;
	struct snd_timer_instance *slave, *tmp;

	if (snd_BUG_ON(!timeri))
		return -ENXIO;

	mutex_lock(&register_mutex);
	list_del(&timeri->open_list);

	/* force to stop the timer */
	snd_timer_stop(timeri);

	timer = timeri->timer;
	if (timer) {
		/* wait, until the active callback is finished */
		spin_lock_irq(&timer->lock);
		while (timeri->flags & SNDRV_TIMER_IFLG_CALLBACK) {
			spin_unlock_irq(&timer->lock);
			udelay(10);
			spin_lock_irq(&timer->lock);
		}
		spin_unlock_irq(&timer->lock);

		/* remove slave links */
		spin_lock_irq(&slave_active_lock);
		spin_lock(&timer->lock);
		list_for_each_entry_safe(slave, tmp, &timeri->slave_list_head,
					 open_list) {
			list_move_tail(&slave->open_list, &snd_timer_slave_list);
			slave->master = NULL;
			slave->timer = NULL;
			list_del_init(&slave->ack_list);
			list_del_init(&slave->active_list);
		}
		spin_unlock(&timer->lock);
		spin_unlock_irq(&slave_active_lock);

		/* slave doesn't need to release timer resources below */
		if (timeri->flags & SNDRV_TIMER_IFLG_SLAVE)
			timer = NULL;
	}

	if (timeri->private_free)
		timeri->private_free(timeri);
	kfree(timeri->owner);
	kfree(timeri);

	if (timer) {
		if (list_empty(&timer->open_list_head) && timer->hw.close)
			timer->hw.close(timer);
		/* release a card refcount for safe disconnection */
		if (timer->card)
			put_device(&timer->card->card_dev);
		module_put(timer->module);
	}

	mutex_unlock(&register_mutex);
	return 0;
}