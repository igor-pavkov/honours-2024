static void line6_unlink_audio_urbs(struct snd_line6_pcm *line6pcm,
				    struct line6_pcm_stream *pcms)
{
	int i;

	for (i = 0; i < line6pcm->line6->iso_buffers; i++) {
		if (test_bit(i, &pcms->active_urbs)) {
			if (!test_and_set_bit(i, &pcms->unlink_urbs))
				usb_unlink_urb(pcms->urbs[i]);
		}
	}
}