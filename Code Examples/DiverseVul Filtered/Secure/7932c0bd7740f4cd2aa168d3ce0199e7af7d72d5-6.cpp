void vhost_dev_reset_owner(struct vhost_dev *dev, struct vhost_memory *memory)
{
	int i;

	vhost_dev_cleanup(dev, true);

	/* Restore memory to default empty mapping. */
	memory->nregions = 0;
	dev->memory = memory;
	/* We don't need VQ locks below since vhost_dev_cleanup makes sure
	 * VQs aren't running.
	 */
	for (i = 0; i < dev->nvqs; ++i)
		dev->vqs[i]->memory = memory;
}