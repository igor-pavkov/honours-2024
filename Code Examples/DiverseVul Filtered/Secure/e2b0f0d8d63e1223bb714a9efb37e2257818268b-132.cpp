static gboolean avrcp_get_total_numberofitems_rsp(struct avctp *conn,
					uint8_t *operands, size_t operand_count,
					void *user_data)
{
	struct avrcp_browsing_header *pdu = (void *) operands;
	struct avrcp *session = user_data;
	struct avrcp_player *player = session->controller->player;
	struct media_player *mp = player->user_data;
	uint32_t num_of_items = 0;

	if (pdu == NULL)
		return -ETIMEDOUT;

	if (pdu->params[0] != AVRCP_STATUS_SUCCESS || operand_count < 7)
		return -EINVAL;

	if (pdu->params[0] == AVRCP_STATUS_OUT_OF_BOUNDS)
		goto done;

	player->uid_counter = get_be16(&pdu->params[1]);
	num_of_items = get_be32(&pdu->params[3]);

	if (!num_of_items)
		return -EINVAL;

done:
	media_player_total_items_complete(mp, num_of_items);
	return FALSE;
}