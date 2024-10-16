void snd_card_disconnect_sync(struct snd_card *card)
{
	int err;

	err = snd_card_disconnect(card);
	if (err < 0) {
		dev_err(card->dev,
			"snd_card_disconnect error (%d), skipping sync\n",
			err);
		return;
	}

	spin_lock_irq(&card->files_lock);
	wait_event_lock_irq(card->remove_sleep,
			    list_empty(&card->files_list),
			    card->files_lock);
	spin_unlock_irq(&card->files_lock);
}