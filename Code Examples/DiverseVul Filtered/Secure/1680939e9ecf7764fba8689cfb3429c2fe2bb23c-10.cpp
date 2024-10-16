static struct rpmsg_device *virtio_rpmsg_create_channel(struct rpmsg_device *rpdev,
							struct rpmsg_channel_info *chinfo)
{
	struct virtio_rpmsg_channel *vch = to_virtio_rpmsg_channel(rpdev);
	struct virtproc_info *vrp = vch->vrp;

	return __rpmsg_create_channel(vrp, chinfo);
}