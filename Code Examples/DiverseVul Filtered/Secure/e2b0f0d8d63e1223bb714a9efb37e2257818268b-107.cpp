static gboolean avrcp_play_item_rsp(struct avctp *conn, uint8_t code,
					uint8_t subunit, uint8_t transaction,
					uint8_t *operands, size_t operand_count,
					void *user_data)
{
	struct avrcp_header *pdu = (void *) operands;
	struct avrcp *session = (void *) user_data;
	struct avrcp_player *player = session->controller->player;
	struct media_player *mp = player->user_data;
	int ret = 0;

	if (pdu == NULL) {
		ret = -ETIMEDOUT;
		goto done;
	}

	if (pdu->params[0] != AVRCP_STATUS_SUCCESS) {
		switch (pdu->params[0]) {
		case AVRCP_STATUS_UID_CHANGED:
		case AVRCP_STATUS_DOES_NOT_EXIST:
			ret = -ENOENT;
			break;
		default:
			ret = -EINVAL;
			break;
		}
		goto done;
	}

done:
	media_player_play_item_complete(mp, ret);

	return FALSE;
}