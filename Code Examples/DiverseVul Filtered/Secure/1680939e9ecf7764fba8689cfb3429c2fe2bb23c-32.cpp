static int rpmsg_send_offchannel_raw(struct rpmsg_device *rpdev,
				     u32 src, u32 dst,
				     void *data, int len, bool wait)
{
	struct virtio_rpmsg_channel *vch = to_virtio_rpmsg_channel(rpdev);
	struct virtproc_info *vrp = vch->vrp;
	struct device *dev = &rpdev->dev;
	struct scatterlist sg;
	struct rpmsg_hdr *msg;
	int err;

	/* bcasting isn't allowed */
	if (src == RPMSG_ADDR_ANY || dst == RPMSG_ADDR_ANY) {
		dev_err(dev, "invalid addr (src 0x%x, dst 0x%x)\n", src, dst);
		return -EINVAL;
	}

	/*
	 * We currently use fixed-sized buffers, and therefore the payload
	 * length is limited.
	 *
	 * One of the possible improvements here is either to support
	 * user-provided buffers (and then we can also support zero-copy
	 * messaging), or to improve the buffer allocator, to support
	 * variable-length buffer sizes.
	 */
	if (len > vrp->buf_size - sizeof(struct rpmsg_hdr)) {
		dev_err(dev, "message is too big (%d)\n", len);
		return -EMSGSIZE;
	}

	/* grab a buffer */
	msg = get_a_tx_buf(vrp);
	if (!msg && !wait)
		return -ENOMEM;

	/* no free buffer ? wait for one (but bail after 15 seconds) */
	while (!msg) {
		/* enable "tx-complete" interrupts, if not already enabled */
		rpmsg_upref_sleepers(vrp);

		/*
		 * sleep until a free buffer is available or 15 secs elapse.
		 * the timeout period is not configurable because there's
		 * little point in asking drivers to specify that.
		 * if later this happens to be required, it'd be easy to add.
		 */
		err = wait_event_interruptible_timeout(vrp->sendq,
					(msg = get_a_tx_buf(vrp)),
					msecs_to_jiffies(15000));

		/* disable "tx-complete" interrupts if we're the last sleeper */
		rpmsg_downref_sleepers(vrp);

		/* timeout ? */
		if (!err) {
			dev_err(dev, "timeout waiting for a tx buffer\n");
			return -ERESTARTSYS;
		}
	}

	msg->len = cpu_to_rpmsg16(rpdev, len);
	msg->flags = 0;
	msg->src = cpu_to_rpmsg32(rpdev, src);
	msg->dst = cpu_to_rpmsg32(rpdev, dst);
	msg->reserved = 0;
	memcpy(msg->data, data, len);

	dev_dbg(dev, "TX From 0x%x, To 0x%x, Len %d, Flags %d, Reserved %d\n",
		src, dst, len, msg->flags, msg->reserved);
#if defined(CONFIG_DYNAMIC_DEBUG)
	dynamic_hex_dump("rpmsg_virtio TX: ", DUMP_PREFIX_NONE, 16, 1,
			 msg, sizeof(*msg) + len, true);
#endif

	rpmsg_sg_init(&sg, msg, sizeof(*msg) + len);

	mutex_lock(&vrp->tx_lock);

	/* add message to the remote processor's virtqueue */
	err = virtqueue_add_outbuf(vrp->svq, &sg, 1, msg, GFP_KERNEL);
	if (err) {
		/*
		 * need to reclaim the buffer here, otherwise it's lost
		 * (memory won't leak, but rpmsg won't use it again for TX).
		 * this will wait for a buffer management overhaul.
		 */
		dev_err(dev, "virtqueue_add_outbuf failed: %d\n", err);
		goto out;
	}

	/* tell the remote processor it has a pending message to read */
	virtqueue_kick(vrp->svq);
out:
	mutex_unlock(&vrp->tx_lock);
	return err;
}