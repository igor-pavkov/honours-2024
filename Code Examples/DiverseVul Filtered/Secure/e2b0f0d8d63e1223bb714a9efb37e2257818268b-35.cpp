static gboolean avrcp_get_item_attributes_rsp(struct avctp *conn,
						uint8_t *operands,
						size_t operand_count,
						void *user_data)
{
	struct avrcp *session = user_data;
	struct avrcp_player *player = session->controller->player;
	struct avrcp_browsing_header *pdu = (void *) operands;
	uint8_t count;

	if (pdu == NULL) {
		avrcp_get_element_attributes(session);
		return FALSE;
	}

	if (pdu->params[0] != AVRCP_STATUS_SUCCESS || operand_count < 4) {
		avrcp_get_element_attributes(session);
		return FALSE;
	}

	count = pdu->params[1];

	if (ntohs(pdu->param_len) - 1 < count * 8) {
		error("Invalid parameters");
		return FALSE;
	}

	avrcp_parse_attribute_list(player, &pdu->params[2], count);

	avrcp_get_play_status(session);

	return FALSE;
}