static ssize_t snd_pcm_oss_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
	struct snd_pcm_oss_file *pcm_oss_file;
	struct snd_pcm_substream *substream;
	long result;

	pcm_oss_file = file->private_data;
	substream = pcm_oss_file->streams[SNDRV_PCM_STREAM_PLAYBACK];
	if (substream == NULL)
		return -ENXIO;
	substream->f_flags = file->f_flags & O_NONBLOCK;
	result = snd_pcm_oss_write1(substream, buf, count);
#ifdef OSS_DEBUG
	pcm_dbg(substream->pcm, "pcm_oss: write %li bytes (wrote %li bytes)\n",
	       (long)count, (long)result);
#endif
	return result;
}