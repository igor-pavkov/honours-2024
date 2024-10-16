static int tipc_nl_compat_recv(struct sk_buff *skb, struct genl_info *info)
{
	int err;
	int len;
	struct tipc_nl_compat_msg msg;
	struct nlmsghdr *req_nlh;
	struct nlmsghdr *rep_nlh;
	struct tipc_genlmsghdr *req_userhdr = info->userhdr;

	memset(&msg, 0, sizeof(msg));

	req_nlh = (struct nlmsghdr *)skb->data;
	msg.req = nlmsg_data(req_nlh) + GENL_HDRLEN + TIPC_GENL_HDRLEN;
	msg.cmd = req_userhdr->cmd;
	msg.net = genl_info_net(info);
	msg.dst_sk = skb->sk;

	if ((msg.cmd & 0xC000) && (!netlink_net_capable(skb, CAP_NET_ADMIN))) {
		msg.rep = tipc_get_err_tlv(TIPC_CFG_NOT_NET_ADMIN);
		err = -EACCES;
		goto send;
	}

	len = nlmsg_attrlen(req_nlh, GENL_HDRLEN + TIPC_GENL_HDRLEN);
	if (len && !TLV_OK(msg.req, len)) {
		msg.rep = tipc_get_err_tlv(TIPC_CFG_NOT_SUPPORTED);
		err = -EOPNOTSUPP;
		goto send;
	}

	err = tipc_nl_compat_handle(&msg);
	if ((err == -EOPNOTSUPP) || (err == -EPERM))
		msg.rep = tipc_get_err_tlv(TIPC_CFG_NOT_SUPPORTED);
	else if (err == -EINVAL)
		msg.rep = tipc_get_err_tlv(TIPC_CFG_TLV_ERROR);
send:
	if (!msg.rep)
		return err;

	len = nlmsg_total_size(GENL_HDRLEN + TIPC_GENL_HDRLEN);
	skb_push(msg.rep, len);
	rep_nlh = nlmsg_hdr(msg.rep);
	memcpy(rep_nlh, info->nlhdr, len);
	rep_nlh->nlmsg_len = msg.rep->len;
	genlmsg_unicast(msg.net, msg.rep, NETLINK_CB(skb).portid);

	return err;
}