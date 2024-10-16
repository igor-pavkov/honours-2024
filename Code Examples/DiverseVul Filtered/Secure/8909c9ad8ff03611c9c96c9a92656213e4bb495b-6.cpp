int dev_close(struct net_device *dev)
{
	LIST_HEAD(single);

	list_add(&dev->unreg_list, &single);
	dev_close_many(&single);
	list_del(&single);
	return 0;
}