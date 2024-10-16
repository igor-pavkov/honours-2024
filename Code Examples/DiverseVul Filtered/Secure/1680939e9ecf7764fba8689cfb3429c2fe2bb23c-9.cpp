static int virtio_rpmsg_send_offchannel(struct rpmsg_endpoint *ept, u32 src,
					u32 dst, void *data, int len)
{
	struct rpmsg_device *rpdev = ept->rpdev;

	return rpmsg_send_offchannel_raw(rpdev, src, dst, data, len, true);
}