static u8 ncp_reply_byte(struct ncp_server *server, int offset)
{
	return *(const u8 *)ncp_reply_data(server, offset);
}