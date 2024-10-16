static int snd_pcm_oss_mmap(struct file *file, struct vm_area_struct *area)
{
	struct snd_pcm_oss_file *pcm_oss_file;
	struct snd_pcm_substream *substream = NULL;
	struct snd_pcm_runtime *runtime;
	int err;

#ifdef OSS_DEBUG
	pr_debug("pcm_oss: mmap begin\n");
#endif
	pcm_oss_file = file->private_data;
	switch ((area->vm_flags & (VM_READ | VM_WRITE))) {
	case VM_READ | VM_WRITE:
		substream = pcm_oss_file->streams[SNDRV_PCM_STREAM_PLAYBACK];
		if (substream)
			break;
		fallthrough;
	case VM_READ:
		substream = pcm_oss_file->streams[SNDRV_PCM_STREAM_CAPTURE];
		break;
	case VM_WRITE:
		substream = pcm_oss_file->streams[SNDRV_PCM_STREAM_PLAYBACK];
		break;
	default:
		return -EINVAL;
	}
	/* set VM_READ access as well to fix memset() routines that do
	   reads before writes (to improve performance) */
	area->vm_flags |= VM_READ;
	if (substream == NULL)
		return -ENXIO;
	runtime = substream->runtime;
	if (!(runtime->info & SNDRV_PCM_INFO_MMAP_VALID))
		return -EIO;
	if (runtime->info & SNDRV_PCM_INFO_INTERLEAVED)
		runtime->access = SNDRV_PCM_ACCESS_MMAP_INTERLEAVED;
	else
		return -EIO;
	
	if (runtime->oss.params) {
		/* use mutex_trylock() for params_lock for avoiding a deadlock
		 * between mmap_lock and params_lock taken by
		 * copy_from/to_user() in snd_pcm_oss_write/read()
		 */
		err = snd_pcm_oss_change_params(substream, true);
		if (err < 0)
			return err;
	}
#ifdef CONFIG_SND_PCM_OSS_PLUGINS
	if (runtime->oss.plugin_first != NULL)
		return -EIO;
#endif

	if (area->vm_pgoff != 0)
		return -EINVAL;

	err = snd_pcm_mmap_data(substream, file, area);
	if (err < 0)
		return err;
	runtime->oss.mmap_bytes = area->vm_end - area->vm_start;
	runtime->silence_threshold = 0;
	runtime->silence_size = 0;
#ifdef OSS_DEBUG
	pr_debug("pcm_oss: mmap ok, bytes = 0x%x\n",
	       runtime->oss.mmap_bytes);
#endif
	/* In mmap mode we never stop */
	runtime->stop_threshold = runtime->boundary;

	return 0;
}