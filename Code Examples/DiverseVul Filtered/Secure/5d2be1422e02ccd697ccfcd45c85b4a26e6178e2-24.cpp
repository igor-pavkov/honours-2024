static int tipc_nl_compat_dumpit(struct tipc_nl_compat_cmd_dump *cmd,
				 struct tipc_nl_compat_msg *msg)
{
	int err;
	struct sk_buff *arg;

	if (msg->req_type && !TLV_CHECK_TYPE(msg->req, msg->req_type))
		return -EINVAL;

	msg->rep = tipc_tlv_alloc(msg->rep_size);
	if (!msg->rep)
		return -ENOMEM;

	if (msg->rep_type)
		tipc_tlv_init(msg->rep, msg->rep_type);

	if (cmd->header)
		(*cmd->header)(msg);

	arg = nlmsg_new(0, GFP_KERNEL);
	if (!arg) {
		kfree_skb(msg->rep);
		return -ENOMEM;
	}

	err = __tipc_nl_compat_dumpit(cmd, msg, arg);
	if (err)
		kfree_skb(msg->rep);

	kfree_skb(arg);

	return err;
}