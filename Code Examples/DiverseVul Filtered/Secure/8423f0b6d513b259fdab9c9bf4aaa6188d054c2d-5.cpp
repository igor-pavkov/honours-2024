static int snd_pcm_oss_open_file(struct file *file,
				 struct snd_pcm *pcm,
				 struct snd_pcm_oss_file **rpcm_oss_file,
				 int minor,
				 struct snd_pcm_oss_setup *setup)
{
	int idx, err;
	struct snd_pcm_oss_file *pcm_oss_file;
	struct snd_pcm_substream *substream;
	fmode_t f_mode = file->f_mode;

	if (rpcm_oss_file)
		*rpcm_oss_file = NULL;

	pcm_oss_file = kzalloc(sizeof(*pcm_oss_file), GFP_KERNEL);
	if (pcm_oss_file == NULL)
		return -ENOMEM;

	if ((f_mode & (FMODE_WRITE|FMODE_READ)) == (FMODE_WRITE|FMODE_READ) &&
	    (pcm->info_flags & SNDRV_PCM_INFO_HALF_DUPLEX))
		f_mode = FMODE_WRITE;

	file->f_flags &= ~O_APPEND;
	for (idx = 0; idx < 2; idx++) {
		if (setup[idx].disable)
			continue;
		if (! pcm->streams[idx].substream_count)
			continue; /* no matching substream */
		if (idx == SNDRV_PCM_STREAM_PLAYBACK) {
			if (! (f_mode & FMODE_WRITE))
				continue;
		} else {
			if (! (f_mode & FMODE_READ))
				continue;
		}
		err = snd_pcm_open_substream(pcm, idx, file, &substream);
		if (err < 0) {
			snd_pcm_oss_release_file(pcm_oss_file);
			return err;
		}

		pcm_oss_file->streams[idx] = substream;
		snd_pcm_oss_init_substream(substream, &setup[idx], minor);
	}
	
	if (!pcm_oss_file->streams[0] && !pcm_oss_file->streams[1]) {
		snd_pcm_oss_release_file(pcm_oss_file);
		return -EINVAL;
	}

	file->private_data = pcm_oss_file;
	if (rpcm_oss_file)
		*rpcm_oss_file = pcm_oss_file;
	return 0;
}