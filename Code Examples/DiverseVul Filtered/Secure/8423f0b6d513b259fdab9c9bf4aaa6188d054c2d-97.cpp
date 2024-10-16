static void snd_pcm_oss_proc_free_setup_list(struct snd_pcm_str * pstr)
{
	struct snd_pcm_oss_setup *setup, *setupn;

	for (setup = pstr->oss.setup_list, pstr->oss.setup_list = NULL;
	     setup; setup = setupn) {
		setupn = setup->next;
		kfree(setup->task_name);
		kfree(setup);
	}
	pstr->oss.setup_list = NULL;
}