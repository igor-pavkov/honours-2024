static ssize_t virtio_rpmsg_get_mtu(struct rpmsg_endpoint *ept)
{
	struct rpmsg_device *rpdev = ept->rpdev;
	struct virtio_rpmsg_channel *vch = to_virtio_rpmsg_channel(rpdev);

	return vch->vrp->buf_size - sizeof(struct rpmsg_hdr);
}