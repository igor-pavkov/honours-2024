static void snd_usb_audio_free(struct snd_card *card)
{
	struct snd_usb_audio *chip = card->private_data;
	struct snd_usb_endpoint *ep, *n;

	list_for_each_entry_safe(ep, n, &chip->ep_list, list)
		snd_usb_endpoint_free(ep);

	mutex_destroy(&chip->mutex);
	if (!atomic_read(&chip->shutdown))
		dev_set_drvdata(&chip->dev->dev, NULL);
}