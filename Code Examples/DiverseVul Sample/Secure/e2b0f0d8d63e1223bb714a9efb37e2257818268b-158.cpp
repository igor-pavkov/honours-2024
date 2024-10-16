static int ct_release(struct avrcp_player *player)
{
	struct avrcp *session = player->sessions->data;

	if (session == NULL)
		return -ENOTCONN;

	set_ct_player(session, player);

	return avctp_send_release_passthrough(session->conn);
}