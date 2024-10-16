int snd_line6_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct snd_line6_pcm *line6pcm = snd_pcm_substream_chip(substream);
	struct snd_pcm_substream *s;
	int err;

	clear_bit(LINE6_FLAG_PREPARED, &line6pcm->flags);

	snd_pcm_group_for_each_entry(s, substream) {
		if (s->pcm->card != substream->pcm->card)
			continue;

		switch (cmd) {
		case SNDRV_PCM_TRIGGER_START:
		case SNDRV_PCM_TRIGGER_RESUME:
			if (s->stream == SNDRV_PCM_STREAM_CAPTURE &&
				(line6pcm->line6->properties->capabilities &
					LINE6_CAP_IN_NEEDS_OUT)) {
				err = line6_stream_start(line6pcm, SNDRV_PCM_STREAM_PLAYBACK,
						 LINE6_STREAM_CAPTURE_HELPER);
				if (err < 0)
					return err;
			}
			err = line6_stream_start(line6pcm, s->stream,
						 LINE6_STREAM_PCM);
			if (err < 0)
				return err;
			break;

		case SNDRV_PCM_TRIGGER_STOP:
		case SNDRV_PCM_TRIGGER_SUSPEND:
			if (s->stream == SNDRV_PCM_STREAM_CAPTURE &&
				(line6pcm->line6->properties->capabilities &
					LINE6_CAP_IN_NEEDS_OUT)) {
				line6_stream_stop(line6pcm, SNDRV_PCM_STREAM_PLAYBACK,
					  LINE6_STREAM_CAPTURE_HELPER);
			}
			line6_stream_stop(line6pcm, s->stream,
					  LINE6_STREAM_PCM);
			break;

		case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
			if (s->stream != SNDRV_PCM_STREAM_PLAYBACK)
				return -EINVAL;
			set_bit(LINE6_FLAG_PAUSE_PLAYBACK, &line6pcm->flags);
			break;

		case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
			if (s->stream != SNDRV_PCM_STREAM_PLAYBACK)
				return -EINVAL;
			clear_bit(LINE6_FLAG_PAUSE_PLAYBACK, &line6pcm->flags);
			break;

		default:
			return -EINVAL;
		}
	}

	return 0;
}