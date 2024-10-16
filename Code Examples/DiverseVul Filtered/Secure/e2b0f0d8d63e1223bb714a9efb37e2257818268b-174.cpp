static int ct_hold(struct avrcp_player *player, uint8_t op)
{
	struct avrcp *session = player->sessions->data;

	if (session == NULL)
		return -ENOTCONN;

	set_ct_player(session, player);

	return avctp_send_passthrough(session->conn, op, true);
}