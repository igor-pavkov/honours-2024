static void avrcp_handle_get_folder_items(struct avrcp *session,
				struct avrcp_browsing_header *pdu,
				uint8_t transaction)
{
	uint32_t start_item = 0;
	uint32_t end_item = 0;
	uint8_t scope;
	uint8_t status = AVRCP_STATUS_SUCCESS;

	if (ntohs(pdu->param_len) < 10) {
		status = AVRCP_STATUS_INVALID_PARAM;
		goto failed;
	}

	scope = pdu->params[0];
	start_item = bt_get_be32(&pdu->params[1]);
	end_item = bt_get_be32(&pdu->params[5]);

	DBG("scope 0x%02x start_item 0x%08x end_item 0x%08x", scope,
				start_item, end_item);

	if (end_item < start_item) {
		status = AVRCP_STATUS_INVALID_PARAM;
		goto failed;
	}

	switch (scope) {
	case AVRCP_SCOPE_MEDIA_PLAYER_LIST:
		avrcp_handle_media_player_list(session, pdu,
						start_item, end_item);
		break;
	case AVRCP_SCOPE_MEDIA_PLAYER_VFS:
	case AVRCP_SCOPE_SEARCH:
	case AVRCP_SCOPE_NOW_PLAYING:
	default:
		status = AVRCP_STATUS_INVALID_SCOPE;
		goto failed;
	}

	return;

failed:
	pdu->params[0] = status;
	pdu->param_len = htons(1);
}