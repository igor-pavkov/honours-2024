static gboolean avrcp_get_play_status_rsp(struct avctp *conn, uint8_t code,
					uint8_t subunit, uint8_t transaction,
					uint8_t *operands, size_t operand_count,
					void *user_data)
{
	struct avrcp *session = user_data;
	struct avrcp_player *player = session->controller->player;
	struct media_player *mp = player->user_data;
	struct avrcp_header *pdu = (void *) operands;
	uint32_t duration;
	uint32_t position;
	uint8_t status;

	if (pdu == NULL || code == AVC_CTYPE_REJECTED ||
						ntohs(pdu->params_len) != 9)
		return FALSE;

	memcpy(&duration, pdu->params, sizeof(uint32_t));
	duration = ntohl(duration);
	media_player_set_duration(mp, duration);

	memcpy(&position, pdu->params + 4, sizeof(uint32_t));
	position = ntohl(position);
	media_player_set_position(mp, position);

	memcpy(&status, pdu->params + 8, sizeof(uint8_t));
	media_player_set_status(mp, status_to_string(status));

	return FALSE;
}