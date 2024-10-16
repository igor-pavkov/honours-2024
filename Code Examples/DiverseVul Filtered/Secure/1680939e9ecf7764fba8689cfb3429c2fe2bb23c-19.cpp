static struct rpmsg_endpoint *virtio_rpmsg_create_ept(struct rpmsg_device *rpdev,
						      rpmsg_rx_cb_t cb,
						      void *priv,
						      struct rpmsg_channel_info chinfo)
{
	struct virtio_rpmsg_channel *vch = to_virtio_rpmsg_channel(rpdev);

	return __rpmsg_create_ept(vch->vrp, rpdev, cb, priv, chinfo.src);
}