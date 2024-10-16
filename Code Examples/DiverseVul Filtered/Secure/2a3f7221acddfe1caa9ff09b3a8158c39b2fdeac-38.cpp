int snd_card_add_dev_attr(struct snd_card *card,
			  const struct attribute_group *group)
{
	int i;

	/* loop for (arraysize-1) here to keep NULL at the last entry */
	for (i = 0; i < ARRAY_SIZE(card->dev_groups) - 1; i++) {
		if (!card->dev_groups[i]) {
			card->dev_groups[i] = group;
			return 0;
		}
	}

	dev_err(card->dev, "Too many groups assigned\n");
	return -ENOSPC;
}