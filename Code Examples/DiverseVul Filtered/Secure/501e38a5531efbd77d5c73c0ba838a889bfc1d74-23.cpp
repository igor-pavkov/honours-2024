static struct dev_data *dev_new (void)
{
	struct dev_data		*dev;

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return NULL;
	dev->state = STATE_DEV_DISABLED;
	refcount_set (&dev->count, 1);
	spin_lock_init (&dev->lock);
	INIT_LIST_HEAD (&dev->epfiles);
	init_waitqueue_head (&dev->wait);
	return dev;
}