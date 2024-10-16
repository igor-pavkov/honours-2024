void line6_pcm_disconnect(struct snd_line6_pcm *line6pcm)
{
	line6_unlink_audio_urbs(line6pcm, &line6pcm->out);
	line6_unlink_audio_urbs(line6pcm, &line6pcm->in);
	line6_wait_clear_audio_urbs(line6pcm, &line6pcm->out);
	line6_wait_clear_audio_urbs(line6pcm, &line6pcm->in);
}