static void rpmsg_recv_done(struct virtqueue *rvq)
{
	struct virtproc_info *vrp = rvq->vdev->priv;
	struct device *dev = &rvq->vdev->dev;
	struct rpmsg_hdr *msg;
	unsigned int len, msgs_received = 0;
	int err;

	msg = virtqueue_get_buf(rvq, &len);
	if (!msg) {
		dev_err(dev, "uhm, incoming signal, but no used buffer ?\n");
		return;
	}

	while (msg) {
		err = rpmsg_recv_single(vrp, dev, msg, len);
		if (err)
			break;

		msgs_received++;

		msg = virtqueue_get_buf(rvq, &len);
	}

	dev_dbg(dev, "Received %u messages\n", msgs_received);

	/* tell the remote processor we added another available rx buffer */
	if (msgs_received)
		virtqueue_kick(vrp->rvq);
}