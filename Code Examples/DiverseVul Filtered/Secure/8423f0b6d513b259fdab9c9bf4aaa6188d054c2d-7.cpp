static void snd_pcm_oss_look_for_setup(struct snd_pcm *pcm, int stream,
				      const char *task_name,
				      struct snd_pcm_oss_setup *rsetup)
{
	struct snd_pcm_oss_setup *setup;

	mutex_lock(&pcm->streams[stream].oss.setup_mutex);
	do {
		for (setup = pcm->streams[stream].oss.setup_list; setup;
		     setup = setup->next) {
			if (!strcmp(setup->task_name, task_name))
				goto out;
		}
	} while ((task_name = strip_task_path(task_name)) != NULL);
 out:
	if (setup)
		*rsetup = *setup;
	mutex_unlock(&pcm->streams[stream].oss.setup_mutex);
}