static int tipc_nl_compat_net_dump(struct tipc_nl_compat_msg *msg,
				   struct nlattr **attrs)
{
	__be32 id;
	struct nlattr *net[TIPC_NLA_NET_MAX + 1];
	int err;

	if (!attrs[TIPC_NLA_NET])
		return -EINVAL;

	err = nla_parse_nested(net, TIPC_NLA_NET_MAX, attrs[TIPC_NLA_NET],
			       NULL);
	if (err)
		return err;

	id = htonl(nla_get_u32(net[TIPC_NLA_NET_ID]));

	return tipc_add_tlv(msg->rep, TIPC_TLV_UNSIGNED, &id, sizeof(id));
}