static void line6_cleanup_pcm(struct snd_pcm *pcm)
{
	struct snd_line6_pcm *line6pcm = snd_pcm_chip(pcm);

	cleanup_urbs(&line6pcm->out, line6pcm->line6->iso_buffers);
	cleanup_urbs(&line6pcm->in, line6pcm->line6->iso_buffers);
	kfree(line6pcm);
}