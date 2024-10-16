static gboolean avrcp_get_element_attributes_rsp(struct avctp *conn,
						uint8_t code, uint8_t subunit,
						uint8_t transaction,
						uint8_t *operands,
						size_t operand_count,
						void *user_data)
{
	struct avrcp *session = user_data;
	struct avrcp_player *player = session->controller->player;
	struct avrcp_header *pdu = (void *) operands;
	uint8_t count;

	if (code == AVC_CTYPE_REJECTED)
		return FALSE;

	count = pdu->params[0];

	if (ntohs(pdu->params_len) - 1 < count * 8) {
		error("Invalid parameters");
		return FALSE;
	}

	avrcp_parse_attribute_list(player, &pdu->params[1], count);

	avrcp_get_play_status(session);

	return FALSE;
}