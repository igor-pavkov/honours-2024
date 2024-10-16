static int vhost_user_set_vring_err(struct virtio_net **pdev __rte_unused,
			struct VhostUserMsg *msg,
			int main_fd __rte_unused)
{
	int expected_fds;

	expected_fds = (msg->payload.u64 & VHOST_USER_VRING_NOFD_MASK) ? 0 : 1;
	if (validate_msg_fds(msg, expected_fds) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	if (!(msg->payload.u64 & VHOST_USER_VRING_NOFD_MASK))
		close(msg->fds[0]);
	VHOST_LOG_CONFIG(INFO, "not implemented\n");

	return RTE_VHOST_MSG_RESULT_OK;
}