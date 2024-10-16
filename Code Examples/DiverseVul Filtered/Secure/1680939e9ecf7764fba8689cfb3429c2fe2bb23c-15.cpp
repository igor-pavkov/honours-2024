static int virtio_rpmsg_release_channel(struct rpmsg_device *rpdev,
					struct rpmsg_channel_info *chinfo)
{
	struct virtio_rpmsg_channel *vch = to_virtio_rpmsg_channel(rpdev);
	struct virtproc_info *vrp = vch->vrp;

	return rpmsg_unregister_device(&vrp->vdev->dev, chinfo);
}