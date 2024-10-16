static void line6_buffer_release(struct snd_line6_pcm *line6pcm,
				 struct line6_pcm_stream *pstr, int type)
{
	clear_bit(type, &pstr->opened);
	if (!pstr->opened) {
		line6_wait_clear_audio_urbs(line6pcm, pstr);
		kfree(pstr->buffer);
		pstr->buffer = NULL;
	}
}