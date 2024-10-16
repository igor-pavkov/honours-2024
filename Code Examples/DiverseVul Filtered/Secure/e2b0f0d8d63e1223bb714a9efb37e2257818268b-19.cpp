static uint8_t avrcp_handle_get_element_attributes(struct avrcp *session,
						struct avrcp_header *pdu,
						uint8_t transaction)
{
	struct avrcp_player *player = target_get_player(session);
	uint16_t len = ntohs(pdu->params_len);
	uint64_t identifier = get_le64(&pdu->params[0]);
	uint16_t pos;
	uint8_t nattr;
	GList *attr_ids;
	uint16_t offset;

	if (len < 9 || identifier != 0)
		goto err;

	nattr = pdu->params[8];

	if (len < nattr * sizeof(uint32_t) + 1)
		goto err;

	if (!nattr) {
		/*
		 * Return all available information, at least
		 * title must be returned if there's a track selected.
		 */
		attr_ids = player_list_metadata(player);
		len = g_list_length(attr_ids);
	} else {
		unsigned int i;
		for (i = 0, len = 0, attr_ids = NULL; i < nattr; i++) {
			uint32_t id;

			id = get_be32(&pdu->params[9] + (i * sizeof(id)));

			/* Don't add invalid attributes */
			if (id == AVRCP_MEDIA_ATTRIBUTE_ILLEGAL ||
					id > AVRCP_MEDIA_ATTRIBUTE_LAST)
				continue;

			len++;
			attr_ids = g_list_prepend(attr_ids,
							GUINT_TO_POINTER(id));
		}

		attr_ids = g_list_reverse(attr_ids);
	}

	if (!len)
		goto err;

	session_abort_pending_pdu(session);
	pos = 1;
	offset = 0;
	attr_ids = player_fill_media_attribute(player, attr_ids, pdu->params,
								&pos, &offset);

	if (attr_ids != NULL) {
		session->pending_pdu = pending_pdu_new(pdu->pdu_id, attr_ids,
								offset);
		pdu->packet_type = AVRCP_PACKET_TYPE_START;
	}

	pdu->params[0] = len;
	pdu->params_len = htons(pos);

	return AVC_CTYPE_STABLE;
err:
	pdu->params_len = htons(1);
	pdu->params[0] = AVRCP_STATUS_INVALID_PARAM;
	return AVC_CTYPE_REJECTED;
}