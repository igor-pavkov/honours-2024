static gboolean avrcp_set_addressed_player_rsp(struct avctp *conn, uint8_t code,
					uint8_t subunit, uint8_t transaction,
					uint8_t *operands, size_t operand_count,
					void *user_data)
{
	struct avrcp *session = user_data;
	struct avrcp_player *player = session->controller->player;
	struct avrcp_header *pdu = (void *) operands;

	if (!pdu || code != AVC_CTYPE_ACCEPTED)
		return FALSE;

	player->addressed = true;

	return FALSE;
}