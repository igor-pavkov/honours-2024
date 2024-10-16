static int __tipc_nl_compat_link_set(struct sk_buff *skb,
				     struct tipc_nl_compat_msg *msg)
{
	struct nlattr *prop;
	struct nlattr *link;
	struct tipc_link_config *lc;

	lc = (struct tipc_link_config *)TLV_DATA(msg->req);

	link = nla_nest_start(skb, TIPC_NLA_LINK);
	if (!link)
		return -EMSGSIZE;

	if (nla_put_string(skb, TIPC_NLA_LINK_NAME, lc->name))
		return -EMSGSIZE;

	prop = nla_nest_start(skb, TIPC_NLA_LINK_PROP);
	if (!prop)
		return -EMSGSIZE;

	__tipc_add_link_prop(skb, msg, lc);
	nla_nest_end(skb, prop);
	nla_nest_end(skb, link);

	return 0;
}