int __init snd_card_info_init(void)
{
	struct snd_info_entry *entry;

	entry = snd_info_create_module_entry(THIS_MODULE, "cards", NULL);
	if (! entry)
		return -ENOMEM;
	entry->c.text.read = snd_card_info_read;
	if (snd_info_register(entry) < 0)
		return -ENOMEM; /* freed in error path */

#ifdef MODULE
	entry = snd_info_create_module_entry(THIS_MODULE, "modules", NULL);
	if (!entry)
		return -ENOMEM;
	entry->c.text.read = snd_card_module_info_read;
	if (snd_info_register(entry) < 0)
		return -ENOMEM; /* freed in error path */
#endif

	return 0;
}