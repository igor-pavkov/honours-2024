snd_seq_oss_synth_reset(struct seq_oss_devinfo *dp, int dev)
{
	struct seq_oss_synth *rec;
	struct seq_oss_synthinfo *info;

	snd_assert(dev >= 0 && dev < dp->max_synthdev, return);
	info = &dp->synths[dev];
	if (! info->opened)
		return;
	if (info->sysex)
		info->sysex->len = 0; /* reset sysex */
	reset_channels(info);
	if (info->is_midi) {
		if (midi_synth_dev.opened <= 0)
			return;
		snd_seq_oss_midi_reset(dp, info->midi_mapped);
		/* reopen the device */
		snd_seq_oss_midi_close(dp, dev);
		if (snd_seq_oss_midi_open(dp, info->midi_mapped,
					  dp->file_mode) < 0) {
			midi_synth_dev.opened--;
			info->opened = 0;
			kfree(info->sysex);
			info->sysex = NULL;
			kfree(info->ch);
			info->ch = NULL;
		}
		return;
	}

	rec = get_sdev(dev);
	if (rec == NULL)
		return;
	if (rec->oper.reset) {
		rec->oper.reset(&info->arg);
	} else {
		struct snd_seq_event ev;
		memset(&ev, 0, sizeof(ev));
		snd_seq_oss_fill_addr(dp, &ev, info->arg.addr.client,
				      info->arg.addr.port);
		ev.type = SNDRV_SEQ_EVENT_RESET;
		snd_seq_oss_dispatch(dp, &ev, 0, 0);
	}
	snd_use_lock_free(&rec->use_lock);
}