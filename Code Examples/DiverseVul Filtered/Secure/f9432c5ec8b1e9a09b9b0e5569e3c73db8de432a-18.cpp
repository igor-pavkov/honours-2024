static struct rfcomm_dev *__rfcomm_dev_get(int id)
{
	struct rfcomm_dev *dev;

	list_for_each_entry(dev, &rfcomm_dev_list, list)
		if (dev->id == id)
			return dev;

	return NULL;
}