int snd_line6_prepare(struct snd_pcm_substream *substream)
{
	struct snd_line6_pcm *line6pcm = snd_pcm_substream_chip(substream);
	struct line6_pcm_stream *pstr = get_stream(line6pcm, substream->stream);

	mutex_lock(&line6pcm->state_mutex);
	if (!pstr->running)
		line6_wait_clear_audio_urbs(line6pcm, pstr);

	if (!test_and_set_bit(LINE6_FLAG_PREPARED, &line6pcm->flags)) {
		line6pcm->out.count = 0;
		line6pcm->out.pos = 0;
		line6pcm->out.pos_done = 0;
		line6pcm->out.bytes = 0;
		line6pcm->in.count = 0;
		line6pcm->in.pos_done = 0;
		line6pcm->in.bytes = 0;
	}

	mutex_unlock(&line6pcm->state_mutex);
	return 0;
}