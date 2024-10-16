static gboolean avrcp_get_media_player_list_rsp(struct avctp *conn,
						uint8_t *operands,
						size_t operand_count,
						void *user_data)
{
	struct avrcp_browsing_header *pdu = (void *) operands;
	struct avrcp *session = user_data;
	uint16_t count;
	size_t i;
	GSList *removed;

	if (pdu == NULL || pdu->params[0] != AVRCP_STATUS_SUCCESS ||
							operand_count < 5)
		return FALSE;

	removed = g_slist_copy(session->controller->players);
	count = get_be16(&operands[6]);

	for (i = 8; count && i < operand_count; count--) {
		struct avrcp_player *player;
		uint8_t type;
		uint16_t len;

		type = operands[i++];
		len = get_be16(&operands[i]);
		i += 2;

		if (type != 0x01) {
			i += len;
			continue;
		}

		if (i + len > operand_count) {
			error("Invalid player item length");
			return FALSE;
		}

		player = avrcp_parse_media_player_item(session, &operands[i],
									len);
		if (player)
			removed = g_slist_remove(removed, player);

		i += len;
	}

	g_slist_free_full(removed, player_remove);

	/* There should always be an active player */
	if (!session->controller->player)
		create_ct_player(session, 0);

	return FALSE;
}