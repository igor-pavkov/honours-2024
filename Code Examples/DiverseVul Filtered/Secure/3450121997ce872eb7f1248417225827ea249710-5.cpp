int line6_pcm_acquire(struct snd_line6_pcm *line6pcm, int type, bool start)
{
	struct line6_pcm_stream *pstr;
	int ret = 0, dir;

	/* TODO: We should assert SNDRV_PCM_STREAM_PLAYBACK/CAPTURE == 0/1 */
	mutex_lock(&line6pcm->state_mutex);
	for (dir = 0; dir < 2; dir++) {
		pstr = get_stream(line6pcm, dir);
		ret = line6_buffer_acquire(line6pcm, pstr, dir, type);
		if (ret < 0)
			goto error;
		if (!pstr->running)
			line6_wait_clear_audio_urbs(line6pcm, pstr);
	}
	if (start) {
		for (dir = 0; dir < 2; dir++) {
			ret = line6_stream_start(line6pcm, dir, type);
			if (ret < 0)
				goto error;
		}
	}
 error:
	mutex_unlock(&line6pcm->state_mutex);
	if (ret < 0)
		line6_pcm_release(line6pcm, type);
	return ret;
}