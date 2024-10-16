static void avrcp_handle_get_total_number_of_items(struct avrcp *session,
				struct avrcp_browsing_header *pdu,
				uint8_t transaction)
{
	uint8_t scope;
	uint8_t status = AVRCP_STATUS_SUCCESS;

	if (ntohs(pdu->param_len) != 1) {
		status = AVRCP_STATUS_INVALID_PARAM;
		goto failed;
	}

	scope = pdu->params[0];

	switch (scope) {
	case AVRCP_SCOPE_MEDIA_PLAYER_LIST:
		avrcp_handle_media_player_list_num_items(session, pdu);
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