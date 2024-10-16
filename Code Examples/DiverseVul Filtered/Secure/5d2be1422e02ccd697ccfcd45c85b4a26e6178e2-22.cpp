static int __tipc_nl_compat_publ_dump(struct tipc_nl_compat_msg *msg,
				      struct nlattr **attrs)
{
	u32 type, lower, upper;
	struct nlattr *publ[TIPC_NLA_PUBL_MAX + 1];
	int err;

	if (!attrs[TIPC_NLA_PUBL])
		return -EINVAL;

	err = nla_parse_nested(publ, TIPC_NLA_PUBL_MAX, attrs[TIPC_NLA_PUBL],
			       NULL);
	if (err)
		return err;

	type = nla_get_u32(publ[TIPC_NLA_PUBL_TYPE]);
	lower = nla_get_u32(publ[TIPC_NLA_PUBL_LOWER]);
	upper = nla_get_u32(publ[TIPC_NLA_PUBL_UPPER]);

	if (lower == upper)
		tipc_tlv_sprintf(msg->rep, " {%u,%u}", type, lower);
	else
		tipc_tlv_sprintf(msg->rep, " {%u,%u,%u}", type, lower, upper);

	return 0;
}