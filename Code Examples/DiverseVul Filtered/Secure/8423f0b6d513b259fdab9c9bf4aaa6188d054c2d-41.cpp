static void snd_pcm_oss_release_substream(struct snd_pcm_substream *substream)
{
	snd_pcm_oss_release_buffers(substream);
	substream->oss.oss = 0;
}