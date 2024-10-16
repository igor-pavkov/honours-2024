static int virtio_rpmsg_announce_destroy(struct rpmsg_device *rpdev)
{
	struct virtio_rpmsg_channel *vch = to_virtio_rpmsg_channel(rpdev);
	struct virtproc_info *vrp = vch->vrp;
	struct device *dev = &rpdev->dev;
	int err = 0;

	/* tell remote processor's name service we're removing this channel */
	if (rpdev->announce && rpdev->ept &&
	    virtio_has_feature(vrp->vdev, VIRTIO_RPMSG_F_NS)) {
		struct rpmsg_ns_msg nsm;

		strncpy(nsm.name, rpdev->id.name, RPMSG_NAME_SIZE);
		nsm.addr = cpu_to_rpmsg32(rpdev, rpdev->ept->addr);
		nsm.flags = cpu_to_rpmsg32(rpdev, RPMSG_NS_DESTROY);

		err = rpmsg_sendto(rpdev->ept, &nsm, sizeof(nsm), RPMSG_NS_ADDR);
		if (err)
			dev_err(dev, "failed to announce service %d\n", err);
	}

	return err;
}