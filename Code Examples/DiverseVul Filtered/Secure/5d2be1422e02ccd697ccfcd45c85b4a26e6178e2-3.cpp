static int tipc_nl_compat_bearer_enable(struct tipc_nl_compat_cmd_doit *cmd,
					struct sk_buff *skb,
					struct tipc_nl_compat_msg *msg)
{
	struct nlattr *prop;
	struct nlattr *bearer;
	struct tipc_bearer_config *b;

	b = (struct tipc_bearer_config *)TLV_DATA(msg->req);

	bearer = nla_nest_start(skb, TIPC_NLA_BEARER);
	if (!bearer)
		return -EMSGSIZE;

	if (nla_put_string(skb, TIPC_NLA_BEARER_NAME, b->name))
		return -EMSGSIZE;

	if (nla_put_u32(skb, TIPC_NLA_BEARER_DOMAIN, ntohl(b->disc_domain)))
		return -EMSGSIZE;

	if (ntohl(b->priority) <= TIPC_MAX_LINK_PRI) {
		prop = nla_nest_start(skb, TIPC_NLA_BEARER_PROP);
		if (!prop)
			return -EMSGSIZE;
		if (nla_put_u32(skb, TIPC_NLA_PROP_PRIO, ntohl(b->priority)))
			return -EMSGSIZE;
		nla_nest_end(skb, prop);
	}
	nla_nest_end(skb, bearer);

	return 0;
}