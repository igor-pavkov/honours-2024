void cma_dev_get(struct cma_device *cma_dev)
{
	refcount_inc(&cma_dev->refcount);
}