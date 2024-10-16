static int vhost_user_set_log_fd(struct virtio_net **pdev __rte_unused,
			struct VhostUserMsg *msg,
			int main_fd __rte_unused)
{
	if (validate_msg_fds(msg, 1) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	close(msg->fds[0]);
	VHOST_LOG_CONFIG(INFO, "not implemented.\n");

	return RTE_VHOST_MSG_RESULT_OK;
}