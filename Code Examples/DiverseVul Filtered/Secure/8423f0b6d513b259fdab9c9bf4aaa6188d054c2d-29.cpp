static int snd_pcm_oss_open(struct inode *inode, struct file *file)
{
	int err;
	char task_name[32];
	struct snd_pcm *pcm;
	struct snd_pcm_oss_file *pcm_oss_file;
	struct snd_pcm_oss_setup setup[2];
	int nonblock;
	wait_queue_entry_t wait;

	err = nonseekable_open(inode, file);
	if (err < 0)
		return err;

	pcm = snd_lookup_oss_minor_data(iminor(inode),
					SNDRV_OSS_DEVICE_TYPE_PCM);
	if (pcm == NULL) {
		err = -ENODEV;
		goto __error1;
	}
	err = snd_card_file_add(pcm->card, file);
	if (err < 0)
		goto __error1;
	if (!try_module_get(pcm->card->module)) {
		err = -EFAULT;
		goto __error2;
	}
	if (snd_task_name(current, task_name, sizeof(task_name)) < 0) {
		err = -EFAULT;
		goto __error;
	}
	memset(setup, 0, sizeof(setup));
	if (file->f_mode & FMODE_WRITE)
		snd_pcm_oss_look_for_setup(pcm, SNDRV_PCM_STREAM_PLAYBACK,
					   task_name, &setup[0]);
	if (file->f_mode & FMODE_READ)
		snd_pcm_oss_look_for_setup(pcm, SNDRV_PCM_STREAM_CAPTURE,
					   task_name, &setup[1]);

	nonblock = !!(file->f_flags & O_NONBLOCK);
	if (!nonblock)
		nonblock = nonblock_open;

	init_waitqueue_entry(&wait, current);
	add_wait_queue(&pcm->open_wait, &wait);
	mutex_lock(&pcm->open_mutex);
	while (1) {
		err = snd_pcm_oss_open_file(file, pcm, &pcm_oss_file,
					    iminor(inode), setup);
		if (err >= 0)
			break;
		if (err == -EAGAIN) {
			if (nonblock) {
				err = -EBUSY;
				break;
			}
		} else
			break;
		set_current_state(TASK_INTERRUPTIBLE);
		mutex_unlock(&pcm->open_mutex);
		schedule();
		mutex_lock(&pcm->open_mutex);
		if (pcm->card->shutdown) {
			err = -ENODEV;
			break;
		}
		if (signal_pending(current)) {
			err = -ERESTARTSYS;
			break;
		}
	}
	remove_wait_queue(&pcm->open_wait, &wait);
	mutex_unlock(&pcm->open_mutex);
	if (err < 0)
		goto __error;
	snd_card_unref(pcm->card);
	return err;

      __error:
     	module_put(pcm->card->module);
      __error2:
      	snd_card_file_remove(pcm->card, file);
      __error1:
	if (pcm)
		snd_card_unref(pcm->card);
	return err;
}