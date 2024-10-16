static int __tipc_nl_compat_dumpit(struct tipc_nl_compat_cmd_dump *cmd,
				   struct tipc_nl_compat_msg *msg,
				   struct sk_buff *arg)
{
	int len = 0;
	int err;
	struct sk_buff *buf;
	struct nlmsghdr *nlmsg;
	struct netlink_callback cb;

	memset(&cb, 0, sizeof(cb));
	cb.nlh = (struct nlmsghdr *)arg->data;
	cb.skb = arg;

	buf = nlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	buf->sk = msg->dst_sk;

	do {
		int rem;

		len = (*cmd->dumpit)(buf, &cb);

		nlmsg_for_each_msg(nlmsg, nlmsg_hdr(buf), len, rem) {
			struct nlattr **attrs;

			err = tipc_nlmsg_parse(nlmsg, &attrs);
			if (err)
				goto err_out;

			err = (*cmd->format)(msg, attrs);
			if (err)
				goto err_out;

			if (tipc_skb_tailroom(msg->rep) <= 1) {
				err = -EMSGSIZE;
				goto err_out;
			}
		}

		skb_reset_tail_pointer(buf);
		buf->len = 0;

	} while (len);

	err = 0;

err_out:
	kfree_skb(buf);

	if (err == -EMSGSIZE) {
		/* The legacy API only considered messages filling
		 * "ULTRA_STRING_MAX_LEN" to be truncated.
		 */
		if ((TIPC_SKB_MAX - msg->rep->len) <= 1) {
			char *tail = skb_tail_pointer(msg->rep);

			if (*tail != '\0')
				sprintf(tail - sizeof(REPLY_TRUNCATED) - 1,
					REPLY_TRUNCATED);
		}

		return 0;
	}

	return err;
}