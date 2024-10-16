static int tipc_nl_compat_name_table_dump_header(struct tipc_nl_compat_msg *msg)
{
	int i;
	u32 depth;
	struct tipc_name_table_query *ntq;
	static const char * const header[] = {
		"Type       ",
		"Lower      Upper      ",
		"Port Identity              ",
		"Publication Scope"
	};

	ntq = (struct tipc_name_table_query *)TLV_DATA(msg->req);

	depth = ntohl(ntq->depth);

	if (depth > 4)
		depth = 4;
	for (i = 0; i < depth; i++)
		tipc_tlv_sprintf(msg->rep, header[i]);
	tipc_tlv_sprintf(msg->rep, "\n");

	return 0;
}