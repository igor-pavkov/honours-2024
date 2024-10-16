int snd_compress_new(struct snd_card *card, int device,
			int dirn, struct snd_compr *compr)
{
	static struct snd_device_ops ops = {
		.dev_free = NULL,
		.dev_register = snd_compress_dev_register,
		.dev_disconnect = snd_compress_dev_disconnect,
	};

	compr->card = card;
	compr->device = device;
	compr->direction = dirn;
	return snd_device_new(card, SNDRV_DEV_COMPRESS, compr, &ops);
}