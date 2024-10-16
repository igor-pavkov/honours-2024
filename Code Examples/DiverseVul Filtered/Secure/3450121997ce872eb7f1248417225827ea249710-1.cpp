int snd_line6_hw_free(struct snd_pcm_substream *substream)
{
	struct snd_line6_pcm *line6pcm = snd_pcm_substream_chip(substream);
	struct line6_pcm_stream *pstr = get_stream(line6pcm, substream->stream);

	mutex_lock(&line6pcm->state_mutex);
	line6_buffer_release(line6pcm, pstr, LINE6_STREAM_PCM);
	mutex_unlock(&line6pcm->state_mutex);
	return snd_pcm_lib_free_pages(substream);
}