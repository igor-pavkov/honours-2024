int snd_pcm_notify(struct snd_pcm_notify *notify, int nfree)
{
	struct snd_pcm *pcm;

	if (snd_BUG_ON(!notify ||
		       !notify->n_register ||
		       !notify->n_unregister ||
		       !notify->n_disconnect))
		return -EINVAL;
	mutex_lock(&register_mutex);
	if (nfree) {
		list_del(&notify->list);
		list_for_each_entry(pcm, &snd_pcm_devices, list)
			notify->n_unregister(pcm);
	} else {
		list_add_tail(&notify->list, &snd_pcm_notify_list);
		list_for_each_entry(pcm, &snd_pcm_devices, list)
			notify->n_register(pcm);
	}
	mutex_unlock(&register_mutex);
	return 0;
}