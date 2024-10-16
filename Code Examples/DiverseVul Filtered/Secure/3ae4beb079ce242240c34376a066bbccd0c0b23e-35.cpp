static int process_slave_message_reply(struct virtio_net *dev,
				       const struct VhostUserMsg *msg)
{
	struct VhostUserMsg msg_reply;
	int ret;

	if ((msg->flags & VHOST_USER_NEED_REPLY) == 0)
		return 0;

	ret = read_vhost_message(dev->slave_req_fd, &msg_reply);
	if (ret <= 0) {
		if (ret < 0)
			VHOST_LOG_CONFIG(ERR,
				"vhost read slave message reply failed\n");
		else
			VHOST_LOG_CONFIG(INFO,
				"vhost peer closed\n");
		ret = -1;
		goto out;
	}

	ret = 0;
	if (msg_reply.request.slave != msg->request.slave) {
		VHOST_LOG_CONFIG(ERR,
			"Received unexpected msg type (%u), expected %u\n",
			msg_reply.request.slave, msg->request.slave);
		ret = -1;
		goto out;
	}

	ret = msg_reply.payload.u64 ? -1 : 0;

out:
	rte_spinlock_unlock(&dev->slave_req_lock);
	return ret;
}