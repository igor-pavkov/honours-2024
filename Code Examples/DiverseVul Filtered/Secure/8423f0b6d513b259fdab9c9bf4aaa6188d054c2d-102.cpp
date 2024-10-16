static int snd_pcm_oss_get_caps(struct snd_pcm_oss_file *pcm_oss_file)
{
	int result, idx;
	
	result = DSP_CAP_TRIGGER | DSP_CAP_MMAP	| DSP_CAP_DUPLEX | DSP_CAP_REALTIME;
	for (idx = 0; idx < 2; idx++) {
		struct snd_pcm_substream *substream = pcm_oss_file->streams[idx];
		result = snd_pcm_oss_get_caps1(substream, result);
	}
	result |= 0x0001;	/* revision - same as SB AWE 64 */
	return result;
}