static void avrcp_playback_pos_changed(struct avrcp *session,
						struct avrcp_header *pdu)
{
	struct avrcp_player *player = session->controller->player;
	struct media_player *mp = player->user_data;
	uint32_t position;

	position = get_be32(&pdu->params[1]);
	media_player_set_position(mp, position);
}