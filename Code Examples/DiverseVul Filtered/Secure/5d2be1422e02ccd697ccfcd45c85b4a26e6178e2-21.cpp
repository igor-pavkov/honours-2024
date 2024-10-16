static int tipc_nl_compat_bearer_disable(struct tipc_nl_compat_cmd_doit *cmd,
					 struct sk_buff *skb,
					 struct tipc_nl_compat_msg *msg)
{
	char *name;
	struct nlattr *bearer;

	name = (char *)TLV_DATA(msg->req);

	bearer = nla_nest_start(skb, TIPC_NLA_BEARER);
	if (!bearer)
		return -EMSGSIZE;

	if (nla_put_string(skb, TIPC_NLA_BEARER_NAME, name))
		return -EMSGSIZE;

	nla_nest_end(skb, bearer);

	return 0;
}