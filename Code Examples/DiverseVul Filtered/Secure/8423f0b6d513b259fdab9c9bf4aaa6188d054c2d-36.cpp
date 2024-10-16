static int snd_pcm_oss_get_mapbuf(struct snd_pcm_oss_file *pcm_oss_file, int stream, struct buffmem_desc __user * _info)
{
	// it won't be probably implemented
	// pr_debug("TODO: snd_pcm_oss_get_mapbuf\n");
	return -EINVAL;
}