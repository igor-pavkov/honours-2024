void snd_device_initialize(struct device *dev, struct snd_card *card)
{
	device_initialize(dev);
	if (card)
		dev->parent = &card->card_dev;
	dev->class = sound_class;
	dev->release = default_release;
}