static int tipc_cmd_show_stats_compat(struct tipc_nl_compat_msg *msg)
{
	msg->rep = tipc_tlv_alloc(ULTRA_STRING_MAX_LEN);
	if (!msg->rep)
		return -ENOMEM;

	tipc_tlv_init(msg->rep, TIPC_TLV_ULTRA_STRING);
	tipc_tlv_sprintf(msg->rep, "TIPC version " TIPC_MOD_VER "\n");

	return 0;
}