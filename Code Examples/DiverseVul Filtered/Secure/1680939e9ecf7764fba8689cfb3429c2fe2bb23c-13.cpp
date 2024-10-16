static void virtio_rpmsg_destroy_ept(struct rpmsg_endpoint *ept)
{
	struct virtio_rpmsg_channel *vch = to_virtio_rpmsg_channel(ept->rpdev);

	__rpmsg_destroy_ept(vch->vrp, ept);
}