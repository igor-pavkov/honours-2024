static int snd_compress_check_input(struct snd_compr_params *params)
{
	/* first let's check the buffer parameter's */
	if (params->buffer.fragment_size == 0 ||
	    params->buffer.fragments > INT_MAX / params->buffer.fragment_size)
		return -EINVAL;

	/* now codec parameters */
	if (params->codec.id == 0 || params->codec.id > SND_AUDIOCODEC_MAX)
		return -EINVAL;

	if (params->codec.ch_in == 0 || params->codec.ch_out == 0)
		return -EINVAL;

	return 0;
}