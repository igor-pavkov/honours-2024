static int tipc_nl_compat_bearer_dump(struct tipc_nl_compat_msg *msg,
				      struct nlattr **attrs)
{
	struct nlattr *bearer[TIPC_NLA_BEARER_MAX + 1];
	int err;

	if (!attrs[TIPC_NLA_BEARER])
		return -EINVAL;

	err = nla_parse_nested(bearer, TIPC_NLA_BEARER_MAX,
			       attrs[TIPC_NLA_BEARER], NULL);
	if (err)
		return err;

	return tipc_add_tlv(msg->rep, TIPC_TLV_BEARER_NAME,
			    nla_data(bearer[TIPC_NLA_BEARER_NAME]),
			    nla_len(bearer[TIPC_NLA_BEARER_NAME]));
}