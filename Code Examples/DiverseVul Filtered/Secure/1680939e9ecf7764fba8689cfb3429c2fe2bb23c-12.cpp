static void rpmsg_virtio_del_ctrl_dev(struct rpmsg_device *rpdev_ctrl)
{
	if (!rpdev_ctrl)
		return;
	kfree(to_virtio_rpmsg_channel(rpdev_ctrl));
}