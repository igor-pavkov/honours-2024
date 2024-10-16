static int timer_set_gparams(struct snd_timer_gparams *gparams)
{
	struct snd_timer *t;
	int err;

	mutex_lock(&register_mutex);
	t = snd_timer_find(&gparams->tid);
	if (!t) {
		err = -ENODEV;
		goto _error;
	}
	if (!list_empty(&t->open_list_head)) {
		err = -EBUSY;
		goto _error;
	}
	if (!t->hw.set_period) {
		err = -ENOSYS;
		goto _error;
	}
	err = t->hw.set_period(t, gparams->period_num, gparams->period_den);
_error:
	mutex_unlock(&register_mutex);
	return err;
}