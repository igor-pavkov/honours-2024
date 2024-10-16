static int tipc_nl_compat_sk_dump(struct tipc_nl_compat_msg *msg,
				  struct nlattr **attrs)
{
	int err;
	u32 sock_ref;
	struct nlattr *sock[TIPC_NLA_SOCK_MAX + 1];

	if (!attrs[TIPC_NLA_SOCK])
		return -EINVAL;

	err = nla_parse_nested(sock, TIPC_NLA_SOCK_MAX, attrs[TIPC_NLA_SOCK],
			       NULL);
	if (err)
		return err;

	sock_ref = nla_get_u32(sock[TIPC_NLA_SOCK_REF]);
	tipc_tlv_sprintf(msg->rep, "%u:", sock_ref);

	if (sock[TIPC_NLA_SOCK_CON]) {
		u32 node;
		struct nlattr *con[TIPC_NLA_CON_MAX + 1];

		nla_parse_nested(con, TIPC_NLA_CON_MAX, sock[TIPC_NLA_SOCK_CON],
				 NULL);

		node = nla_get_u32(con[TIPC_NLA_CON_NODE]);
		tipc_tlv_sprintf(msg->rep, "  connected to <%u.%u.%u:%u>",
				 tipc_zone(node),
				 tipc_cluster(node),
				 tipc_node(node),
				 nla_get_u32(con[TIPC_NLA_CON_SOCK]));

		if (con[TIPC_NLA_CON_FLAG])
			tipc_tlv_sprintf(msg->rep, " via {%u,%u}\n",
					 nla_get_u32(con[TIPC_NLA_CON_TYPE]),
					 nla_get_u32(con[TIPC_NLA_CON_INST]));
		else
			tipc_tlv_sprintf(msg->rep, "\n");
	} else if (sock[TIPC_NLA_SOCK_HAS_PUBL]) {
		tipc_tlv_sprintf(msg->rep, " bound to");

		err = tipc_nl_compat_publ_dump(msg, sock_ref);
		if (err)
			return err;
	}
	tipc_tlv_sprintf(msg->rep, "\n");

	return 0;
}