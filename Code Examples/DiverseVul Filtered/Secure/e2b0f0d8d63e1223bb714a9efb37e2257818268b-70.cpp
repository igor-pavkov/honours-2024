static gboolean avrcp_search_rsp(struct avctp *conn, uint8_t *operands,
					size_t operand_count, void *user_data)
{
	struct avrcp_browsing_header *pdu = (void *) operands;
	struct avrcp *session = (void *) user_data;
	struct avrcp_player *player = session->controller->player;
	struct media_player *mp = player->user_data;
	int ret;

	if (pdu == NULL) {
		ret = -ETIMEDOUT;
		goto done;
	}

	if (pdu->params[0] != AVRCP_STATUS_SUCCESS || operand_count < 7) {
		ret = -EINVAL;
		goto done;
	}

	player->uid_counter = get_be16(&pdu->params[1]);
	ret = get_be32(&pdu->params[3]);

done:
	media_player_search_complete(mp, ret);

	return FALSE;
}