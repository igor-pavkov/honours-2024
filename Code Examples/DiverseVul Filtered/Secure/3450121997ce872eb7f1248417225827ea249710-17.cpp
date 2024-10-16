static int line6_buffer_acquire(struct snd_line6_pcm *line6pcm,
				struct line6_pcm_stream *pstr, int direction, int type)
{
	const int pkt_size =
		(direction == SNDRV_PCM_STREAM_PLAYBACK) ?
			line6pcm->max_packet_size_out :
			line6pcm->max_packet_size_in;

	/* Invoked multiple times in a row so allocate once only */
	if (!test_and_set_bit(type, &pstr->opened) && !pstr->buffer) {
		pstr->buffer =
			kmalloc(array3_size(line6pcm->line6->iso_buffers,
					    LINE6_ISO_PACKETS, pkt_size),
				GFP_KERNEL);
		if (!pstr->buffer)
			return -ENOMEM;
	}
	return 0;
}