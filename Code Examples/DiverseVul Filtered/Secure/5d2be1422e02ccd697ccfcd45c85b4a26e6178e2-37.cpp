static int __tipc_nl_compat_doit(struct tipc_nl_compat_cmd_doit *cmd,
				 struct tipc_nl_compat_msg *msg)
{
	int err;
	struct sk_buff *doit_buf;
	struct sk_buff *trans_buf;
	struct nlattr **attrbuf;
	struct genl_info info;

	trans_buf = alloc_skb(NLMSG_GOODSIZE, GFP_KERNEL);
	if (!trans_buf)
		return -ENOMEM;

	err = (*cmd->transcode)(cmd, trans_buf, msg);
	if (err)
		goto trans_out;

	attrbuf = kmalloc((tipc_genl_family.maxattr + 1) *
			sizeof(struct nlattr *), GFP_KERNEL);
	if (!attrbuf) {
		err = -ENOMEM;
		goto trans_out;
	}

	err = nla_parse(attrbuf, tipc_genl_family.maxattr,
			(const struct nlattr *)trans_buf->data,
			trans_buf->len, NULL);
	if (err)
		goto parse_out;

	doit_buf = alloc_skb(NLMSG_GOODSIZE, GFP_KERNEL);
	if (!doit_buf) {
		err = -ENOMEM;
		goto parse_out;
	}

	doit_buf->sk = msg->dst_sk;

	memset(&info, 0, sizeof(info));
	info.attrs = attrbuf;

	err = (*cmd->doit)(doit_buf, &info);

	kfree_skb(doit_buf);
parse_out:
	kfree(attrbuf);
trans_out:
	kfree_skb(trans_buf);

	return err;
}