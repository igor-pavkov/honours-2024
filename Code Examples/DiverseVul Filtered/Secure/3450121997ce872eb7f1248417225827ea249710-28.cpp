static void line6_stream_stop(struct snd_line6_pcm *line6pcm, int direction,
			  int type)
{
	unsigned long flags;
	struct line6_pcm_stream *pstr = get_stream(line6pcm, direction);

	spin_lock_irqsave(&pstr->lock, flags);
	clear_bit(type, &pstr->running);
	if (!pstr->running) {
		spin_unlock_irqrestore(&pstr->lock, flags);
		line6_unlink_audio_urbs(line6pcm, pstr);
		spin_lock_irqsave(&pstr->lock, flags);
		if (direction == SNDRV_PCM_STREAM_CAPTURE) {
			line6pcm->prev_fbuf = NULL;
			line6pcm->prev_fsize = 0;
		}
	}
	spin_unlock_irqrestore(&pstr->lock, flags);
}