static void cma_remove_one(struct ib_device *device, void *client_data)
{
	struct cma_device *cma_dev = client_data;

	trace_cm_remove_one(device);

	mutex_lock(&lock);
	list_del(&cma_dev->list);
	mutex_unlock(&lock);

	cma_process_remove(cma_dev);
	kfree(cma_dev->default_roce_tos);
	kfree(cma_dev->default_gid_type);
	kfree(cma_dev);
}