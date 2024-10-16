int snd_line6_hw_params(struct snd_pcm_substream *substream,
			struct snd_pcm_hw_params *hw_params)
{
	int ret;
	struct snd_line6_pcm *line6pcm = snd_pcm_substream_chip(substream);
	struct line6_pcm_stream *pstr = get_stream(line6pcm, substream->stream);

	mutex_lock(&line6pcm->state_mutex);
	ret = line6_buffer_acquire(line6pcm, pstr, substream->stream,
	                           LINE6_STREAM_PCM);
	if (ret < 0)
		goto error;

	ret = snd_pcm_lib_malloc_pages(substream,
				       params_buffer_bytes(hw_params));
	if (ret < 0) {
		line6_buffer_release(line6pcm, pstr, LINE6_STREAM_PCM);
		goto error;
	}

	pstr->period = params_period_bytes(hw_params);
 error:
	mutex_unlock(&line6pcm->state_mutex);
	return ret;
}