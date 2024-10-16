static struct rpmsg_device *__rpmsg_create_channel(struct virtproc_info *vrp,
						   struct rpmsg_channel_info *chinfo)
{
	struct virtio_rpmsg_channel *vch;
	struct rpmsg_device *rpdev;
	struct device *tmp, *dev = &vrp->vdev->dev;
	int ret;

	/* make sure a similar channel doesn't already exist */
	tmp = rpmsg_find_device(dev, chinfo);
	if (tmp) {
		/* decrement the matched device's refcount back */
		put_device(tmp);
		dev_err(dev, "channel %s:%x:%x already exist\n",
				chinfo->name, chinfo->src, chinfo->dst);
		return NULL;
	}

	vch = kzalloc(sizeof(*vch), GFP_KERNEL);
	if (!vch)
		return NULL;

	/* Link the channel to our vrp */
	vch->vrp = vrp;

	/* Assign public information to the rpmsg_device */
	rpdev = &vch->rpdev;
	rpdev->src = chinfo->src;
	rpdev->dst = chinfo->dst;
	rpdev->ops = &virtio_rpmsg_ops;
	rpdev->little_endian = virtio_is_little_endian(vrp->vdev);

	/*
	 * rpmsg server channels has predefined local address (for now),
	 * and their existence needs to be announced remotely
	 */
	rpdev->announce = rpdev->src != RPMSG_ADDR_ANY;

	strncpy(rpdev->id.name, chinfo->name, RPMSG_NAME_SIZE);

	rpdev->dev.parent = &vrp->vdev->dev;
	rpdev->dev.release = virtio_rpmsg_release_device;
	ret = rpmsg_register_device(rpdev);
	if (ret)
		return NULL;

	return rpdev;
}