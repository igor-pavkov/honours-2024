static int message_for_md(struct mapped_device *md, unsigned argc, char **argv,
			  char *result, unsigned maxlen)
{
	int r;

	if (**argv != '@')
		return 2; /* no '@' prefix, deliver to target */

	if (!strcasecmp(argv[0], "@cancel_deferred_remove")) {
		if (argc != 1) {
			DMERR("Invalid arguments for @cancel_deferred_remove");
			return -EINVAL;
		}
		return dm_cancel_deferred_remove(md);
	}

	r = dm_stats_message(md, argc, argv, result, maxlen);
	if (r < 2)
		return r;

	DMERR("Unsupported message sent to DM core: %s", argv[0]);
	return -EINVAL;
}