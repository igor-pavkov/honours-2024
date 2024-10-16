static int vhost_user_slave_set_vring_host_notifier(struct virtio_net *dev,
						    int index, int fd,
						    uint64_t offset,
						    uint64_t size)
{
	int ret;
	struct VhostUserMsg msg = {
		.request.slave = VHOST_USER_SLAVE_VRING_HOST_NOTIFIER_MSG,
		.flags = VHOST_USER_VERSION | VHOST_USER_NEED_REPLY,
		.size = sizeof(msg.payload.area),
		.payload.area = {
			.u64 = index & VHOST_USER_VRING_IDX_MASK,
			.size = size,
			.offset = offset,
		},
	};

	if (fd < 0)
		msg.payload.area.u64 |= VHOST_USER_VRING_NOFD_MASK;
	else {
		msg.fds[0] = fd;
		msg.fd_num = 1;
	}

	ret = send_vhost_slave_message(dev, &msg);
	if (ret < 0) {
		VHOST_LOG_CONFIG(ERR,
			"Failed to set host notifier (%d)\n", ret);
		return ret;
	}

	return process_slave_message_reply(dev, &msg);
}