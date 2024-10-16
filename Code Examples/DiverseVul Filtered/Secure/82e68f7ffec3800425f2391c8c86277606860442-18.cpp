snd_seq_oss_synth_cleanup(struct seq_oss_devinfo *dp)
{
	int i;
	struct seq_oss_synth *rec;
	struct seq_oss_synthinfo *info;

	snd_assert(dp->max_synthdev <= SNDRV_SEQ_OSS_MAX_SYNTH_DEVS, return);
	for (i = 0; i < dp->max_synthdev; i++) {
		info = &dp->synths[i];
		if (! info->opened)
			continue;
		if (info->is_midi) {
			if (midi_synth_dev.opened > 0) {
				snd_seq_oss_midi_close(dp, info->midi_mapped);
				midi_synth_dev.opened--;
			}
		} else {
			rec = get_sdev(i);
			if (rec == NULL)
				continue;
			if (rec->opened > 0) {
				debug_printk(("synth %d closed\n", i));
				rec->oper.close(&info->arg);
				module_put(rec->oper.owner);
				rec->opened = 0;
			}
			snd_use_lock_free(&rec->use_lock);
		}
		kfree(info->sysex);
		info->sysex = NULL;
		kfree(info->ch);
		info->ch = NULL;
	}
	dp->synth_opened = 0;
	dp->max_synthdev = 0;
}