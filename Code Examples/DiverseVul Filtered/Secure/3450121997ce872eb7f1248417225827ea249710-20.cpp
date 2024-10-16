static int line6_stream_start(struct snd_line6_pcm *line6pcm, int direction,
			      int type)
{
	unsigned long flags;
	struct line6_pcm_stream *pstr = get_stream(line6pcm, direction);
	int ret = 0;

	spin_lock_irqsave(&pstr->lock, flags);
	if (!test_and_set_bit(type, &pstr->running) &&
	    !(pstr->active_urbs || pstr->unlink_urbs)) {
		pstr->count = 0;
		/* Submit all currently available URBs */
		if (direction == SNDRV_PCM_STREAM_PLAYBACK)
			ret = line6_submit_audio_out_all_urbs(line6pcm);
		else
			ret = line6_submit_audio_in_all_urbs(line6pcm);
	}

	if (ret < 0)
		clear_bit(type, &pstr->running);
	spin_unlock_irqrestore(&pstr->lock, flags);
	return ret;
}