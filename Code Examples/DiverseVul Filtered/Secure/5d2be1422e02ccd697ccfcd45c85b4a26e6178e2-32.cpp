static int tipc_nl_compat_link_reset_stats(struct tipc_nl_compat_cmd_doit *cmd,
					   struct sk_buff *skb,
					   struct tipc_nl_compat_msg *msg)
{
	char *name;
	struct nlattr *link;

	name = (char *)TLV_DATA(msg->req);

	link = nla_nest_start(skb, TIPC_NLA_LINK);
	if (!link)
		return -EMSGSIZE;

	if (nla_put_string(skb, TIPC_NLA_LINK_NAME, name))
		return -EMSGSIZE;

	nla_nest_end(skb, link);

	return 0;
}