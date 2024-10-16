static int tipc_nl_compat_name_table_dump(struct tipc_nl_compat_msg *msg,
					  struct nlattr **attrs)
{
	char port_str[27];
	struct tipc_name_table_query *ntq;
	struct nlattr *nt[TIPC_NLA_NAME_TABLE_MAX + 1];
	struct nlattr *publ[TIPC_NLA_PUBL_MAX + 1];
	u32 node, depth, type, lowbound, upbound;
	static const char * const scope_str[] = {"", " zone", " cluster",
						 " node"};
	int err;

	if (!attrs[TIPC_NLA_NAME_TABLE])
		return -EINVAL;

	err = nla_parse_nested(nt, TIPC_NLA_NAME_TABLE_MAX,
			       attrs[TIPC_NLA_NAME_TABLE], NULL);
	if (err)
		return err;

	if (!nt[TIPC_NLA_NAME_TABLE_PUBL])
		return -EINVAL;

	err = nla_parse_nested(publ, TIPC_NLA_PUBL_MAX,
			       nt[TIPC_NLA_NAME_TABLE_PUBL], NULL);
	if (err)
		return err;

	ntq = (struct tipc_name_table_query *)TLV_DATA(msg->req);

	depth = ntohl(ntq->depth);
	type = ntohl(ntq->type);
	lowbound = ntohl(ntq->lowbound);
	upbound = ntohl(ntq->upbound);

	if (!(depth & TIPC_NTQ_ALLTYPES) &&
	    (type != nla_get_u32(publ[TIPC_NLA_PUBL_TYPE])))
		return 0;
	if (lowbound && (lowbound > nla_get_u32(publ[TIPC_NLA_PUBL_UPPER])))
		return 0;
	if (upbound && (upbound < nla_get_u32(publ[TIPC_NLA_PUBL_LOWER])))
		return 0;

	tipc_tlv_sprintf(msg->rep, "%-10u ",
			 nla_get_u32(publ[TIPC_NLA_PUBL_TYPE]));

	if (depth == 1)
		goto out;

	tipc_tlv_sprintf(msg->rep, "%-10u %-10u ",
			 nla_get_u32(publ[TIPC_NLA_PUBL_LOWER]),
			 nla_get_u32(publ[TIPC_NLA_PUBL_UPPER]));

	if (depth == 2)
		goto out;

	node = nla_get_u32(publ[TIPC_NLA_PUBL_NODE]);
	sprintf(port_str, "<%u.%u.%u:%u>", tipc_zone(node), tipc_cluster(node),
		tipc_node(node), nla_get_u32(publ[TIPC_NLA_PUBL_REF]));
	tipc_tlv_sprintf(msg->rep, "%-26s ", port_str);

	if (depth == 3)
		goto out;

	tipc_tlv_sprintf(msg->rep, "%-10u %s",
			 nla_get_u32(publ[TIPC_NLA_PUBL_KEY]),
			 scope_str[nla_get_u32(publ[TIPC_NLA_PUBL_SCOPE])]);
out:
	tipc_tlv_sprintf(msg->rep, "\n");

	return 0;
}