static ssize_t snd_pcm_oss_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	struct snd_pcm_oss_file *pcm_oss_file;
	struct snd_pcm_substream *substream;

	pcm_oss_file = file->private_data;
	substream = pcm_oss_file->streams[SNDRV_PCM_STREAM_CAPTURE];
	if (substream == NULL)
		return -ENXIO;
	substream->f_flags = file->f_flags & O_NONBLOCK;
#ifndef OSS_DEBUG
	return snd_pcm_oss_read1(substream, buf, count);
#else
	{
		ssize_t res = snd_pcm_oss_read1(substream, buf, count);
		pcm_dbg(substream->pcm,
			"pcm_oss: read %li bytes (returned %li bytes)\n",
			(long)count, (long)res);
		return res;
	}
#endif
}