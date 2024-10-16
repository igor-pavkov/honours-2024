static int tipc_nl_compat_node_dump(struct tipc_nl_compat_msg *msg,
				    struct nlattr **attrs)
{
	struct tipc_node_info node_info;
	struct nlattr *node[TIPC_NLA_NODE_MAX + 1];
	int err;

	if (!attrs[TIPC_NLA_NODE])
		return -EINVAL;

	err = nla_parse_nested(node, TIPC_NLA_NODE_MAX, attrs[TIPC_NLA_NODE],
			       NULL);
	if (err)
		return err;

	node_info.addr = htonl(nla_get_u32(node[TIPC_NLA_NODE_ADDR]));
	node_info.up = htonl(nla_get_flag(node[TIPC_NLA_NODE_UP]));

	return tipc_add_tlv(msg->rep, TIPC_TLV_NODE_INFO, &node_info,
			    sizeof(node_info));
}