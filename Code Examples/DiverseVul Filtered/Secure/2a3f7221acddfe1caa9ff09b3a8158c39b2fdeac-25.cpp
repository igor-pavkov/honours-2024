int snd_card_free(struct snd_card *card)
{
	struct completion released;
	int ret;

	init_completion(&released);
	card->release_completion = &released;
	ret = snd_card_free_when_closed(card);
	if (ret)
		return ret;
	/* wait, until all devices are ready for the free operation */
	wait_for_completion(&released);
	return 0;
}