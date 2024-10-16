const char *blkg_dev_name(struct blkcg_gq *blkg)
{
	/* some drivers (floppy) instantiate a queue w/o disk registered */
	if (blkg->q->backing_dev_info->dev)
		return dev_name(blkg->q->backing_dev_info->dev);
	return NULL;
}