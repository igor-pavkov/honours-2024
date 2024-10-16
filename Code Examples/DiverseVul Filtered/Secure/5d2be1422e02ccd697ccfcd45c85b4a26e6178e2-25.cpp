static int tipc_nl_compat_handle(struct tipc_nl_compat_msg *msg)
{
	struct tipc_nl_compat_cmd_dump dump;
	struct tipc_nl_compat_cmd_doit doit;

	memset(&dump, 0, sizeof(dump));
	memset(&doit, 0, sizeof(doit));

	switch (msg->cmd) {
	case TIPC_CMD_NOOP:
		msg->rep = tipc_tlv_alloc(0);
		if (!msg->rep)
			return -ENOMEM;
		return 0;
	case TIPC_CMD_GET_BEARER_NAMES:
		msg->rep_size = MAX_BEARERS * TLV_SPACE(TIPC_MAX_BEARER_NAME);
		dump.dumpit = tipc_nl_bearer_dump;
		dump.format = tipc_nl_compat_bearer_dump;
		return tipc_nl_compat_dumpit(&dump, msg);
	case TIPC_CMD_ENABLE_BEARER:
		msg->req_type = TIPC_TLV_BEARER_CONFIG;
		doit.doit = tipc_nl_bearer_enable;
		doit.transcode = tipc_nl_compat_bearer_enable;
		return tipc_nl_compat_doit(&doit, msg);
	case TIPC_CMD_DISABLE_BEARER:
		msg->req_type = TIPC_TLV_BEARER_NAME;
		doit.doit = tipc_nl_bearer_disable;
		doit.transcode = tipc_nl_compat_bearer_disable;
		return tipc_nl_compat_doit(&doit, msg);
	case TIPC_CMD_SHOW_LINK_STATS:
		msg->req_type = TIPC_TLV_LINK_NAME;
		msg->rep_size = ULTRA_STRING_MAX_LEN;
		msg->rep_type = TIPC_TLV_ULTRA_STRING;
		dump.dumpit = tipc_nl_node_dump_link;
		dump.format = tipc_nl_compat_link_stat_dump;
		return tipc_nl_compat_dumpit(&dump, msg);
	case TIPC_CMD_GET_LINKS:
		msg->req_type = TIPC_TLV_NET_ADDR;
		msg->rep_size = ULTRA_STRING_MAX_LEN;
		dump.dumpit = tipc_nl_node_dump_link;
		dump.format = tipc_nl_compat_link_dump;
		return tipc_nl_compat_dumpit(&dump, msg);
	case TIPC_CMD_SET_LINK_TOL:
	case TIPC_CMD_SET_LINK_PRI:
	case TIPC_CMD_SET_LINK_WINDOW:
		msg->req_type =  TIPC_TLV_LINK_CONFIG;
		doit.doit = tipc_nl_node_set_link;
		doit.transcode = tipc_nl_compat_link_set;
		return tipc_nl_compat_doit(&doit, msg);
	case TIPC_CMD_RESET_LINK_STATS:
		msg->req_type = TIPC_TLV_LINK_NAME;
		doit.doit = tipc_nl_node_reset_link_stats;
		doit.transcode = tipc_nl_compat_link_reset_stats;
		return tipc_nl_compat_doit(&doit, msg);
	case TIPC_CMD_SHOW_NAME_TABLE:
		msg->req_type = TIPC_TLV_NAME_TBL_QUERY;
		msg->rep_size = ULTRA_STRING_MAX_LEN;
		msg->rep_type = TIPC_TLV_ULTRA_STRING;
		dump.header = tipc_nl_compat_name_table_dump_header;
		dump.dumpit = tipc_nl_name_table_dump;
		dump.format = tipc_nl_compat_name_table_dump;
		return tipc_nl_compat_dumpit(&dump, msg);
	case TIPC_CMD_SHOW_PORTS:
		msg->rep_size = ULTRA_STRING_MAX_LEN;
		msg->rep_type = TIPC_TLV_ULTRA_STRING;
		dump.dumpit = tipc_nl_sk_dump;
		dump.format = tipc_nl_compat_sk_dump;
		return tipc_nl_compat_dumpit(&dump, msg);
	case TIPC_CMD_GET_MEDIA_NAMES:
		msg->rep_size = MAX_MEDIA * TLV_SPACE(TIPC_MAX_MEDIA_NAME);
		dump.dumpit = tipc_nl_media_dump;
		dump.format = tipc_nl_compat_media_dump;
		return tipc_nl_compat_dumpit(&dump, msg);
	case TIPC_CMD_GET_NODES:
		msg->rep_size = ULTRA_STRING_MAX_LEN;
		dump.dumpit = tipc_nl_node_dump;
		dump.format = tipc_nl_compat_node_dump;
		return tipc_nl_compat_dumpit(&dump, msg);
	case TIPC_CMD_SET_NODE_ADDR:
		msg->req_type = TIPC_TLV_NET_ADDR;
		doit.doit = tipc_nl_net_set;
		doit.transcode = tipc_nl_compat_net_set;
		return tipc_nl_compat_doit(&doit, msg);
	case TIPC_CMD_SET_NETID:
		msg->req_type = TIPC_TLV_UNSIGNED;
		doit.doit = tipc_nl_net_set;
		doit.transcode = tipc_nl_compat_net_set;
		return tipc_nl_compat_doit(&doit, msg);
	case TIPC_CMD_GET_NETID:
		msg->rep_size = sizeof(u32);
		dump.dumpit = tipc_nl_net_dump;
		dump.format = tipc_nl_compat_net_dump;
		return tipc_nl_compat_dumpit(&dump, msg);
	case TIPC_CMD_SHOW_STATS:
		return tipc_cmd_show_stats_compat(msg);
	}

	return -EOPNOTSUPP;
}