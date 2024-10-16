struct cma_device *cma_enum_devices_by_ibdev(cma_device_filter	filter,
					     void		*cookie)
{
	struct cma_device *cma_dev;
	struct cma_device *found_cma_dev = NULL;

	mutex_lock(&lock);

	list_for_each_entry(cma_dev, &dev_list, list)
		if (filter(cma_dev->device, cookie)) {
			found_cma_dev = cma_dev;
			break;
		}

	if (found_cma_dev)
		cma_dev_get(found_cma_dev);
	mutex_unlock(&lock);
	return found_cma_dev;
}