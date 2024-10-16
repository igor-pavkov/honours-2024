static int mq_attr_ok(struct ipc_namespace *ipc_ns, struct mq_attr *attr)
{
	int mq_treesize;
	unsigned long total_size;

	if (attr->mq_maxmsg <= 0 || attr->mq_msgsize <= 0)
		return -EINVAL;
	if (capable(CAP_SYS_RESOURCE)) {
		if (attr->mq_maxmsg > HARD_MSGMAX ||
		    attr->mq_msgsize > HARD_MSGSIZEMAX)
			return -EINVAL;
	} else {
		if (attr->mq_maxmsg > ipc_ns->mq_msg_max ||
				attr->mq_msgsize > ipc_ns->mq_msgsize_max)
			return -EINVAL;
	}
	/* check for overflow */
	if (attr->mq_msgsize > ULONG_MAX/attr->mq_maxmsg)
		return -EOVERFLOW;
	mq_treesize = attr->mq_maxmsg * sizeof(struct msg_msg) +
		min_t(unsigned int, attr->mq_maxmsg, MQ_PRIO_MAX) *
		sizeof(struct posix_msg_tree_node);
	total_size = attr->mq_maxmsg * attr->mq_msgsize;
	if (total_size + mq_treesize < total_size)
		return -EOVERFLOW;
	return 0;
}