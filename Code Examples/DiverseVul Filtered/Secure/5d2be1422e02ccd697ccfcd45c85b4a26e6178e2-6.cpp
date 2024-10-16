static int tipc_nl_compat_net_set(struct tipc_nl_compat_cmd_doit *cmd,
				  struct sk_buff *skb,
				  struct tipc_nl_compat_msg *msg)
{
	u32 val;
	struct nlattr *net;

	val = ntohl(*(__be32 *)TLV_DATA(msg->req));

	net = nla_nest_start(skb, TIPC_NLA_NET);
	if (!net)
		return -EMSGSIZE;

	if (msg->cmd == TIPC_CMD_SET_NODE_ADDR) {
		if (nla_put_u32(skb, TIPC_NLA_NET_ADDR, val))
			return -EMSGSIZE;
	} else if (msg->cmd == TIPC_CMD_SET_NETID) {
		if (nla_put_u32(skb, TIPC_NLA_NET_ID, val))
			return -EMSGSIZE;
	}
	nla_nest_end(skb, net);

	return 0;
}