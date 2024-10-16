static void avrcp_uids_changed(struct avrcp *session, struct avrcp_header *pdu)
{
	struct avrcp_player *player = session->controller->player;

	player->uid_counter = get_be16(&pdu->params[1]);
}