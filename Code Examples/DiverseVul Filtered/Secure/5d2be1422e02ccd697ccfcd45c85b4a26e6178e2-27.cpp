static int __tipc_add_link_prop(struct sk_buff *skb,
				struct tipc_nl_compat_msg *msg,
				struct tipc_link_config *lc)
{
	switch (msg->cmd) {
	case TIPC_CMD_SET_LINK_PRI:
		return nla_put_u32(skb, TIPC_NLA_PROP_PRIO, ntohl(lc->value));
	case TIPC_CMD_SET_LINK_TOL:
		return nla_put_u32(skb, TIPC_NLA_PROP_TOL, ntohl(lc->value));
	case TIPC_CMD_SET_LINK_WINDOW:
		return nla_put_u32(skb, TIPC_NLA_PROP_WIN, ntohl(lc->value));
	}

	return -EINVAL;
}