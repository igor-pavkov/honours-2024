static int tipc_nl_compat_doit(struct tipc_nl_compat_cmd_doit *cmd,
			       struct tipc_nl_compat_msg *msg)
{
	int err;

	if (msg->req_type && !TLV_CHECK_TYPE(msg->req, msg->req_type))
		return -EINVAL;

	err = __tipc_nl_compat_doit(cmd, msg);
	if (err)
		return err;

	/* The legacy API considered an empty message a success message */
	msg->rep = tipc_tlv_alloc(0);
	if (!msg->rep)
		return -ENOMEM;

	return 0;
}