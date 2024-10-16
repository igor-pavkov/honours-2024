static gboolean avrcp_change_path_rsp(struct avctp *conn,
					uint8_t *operands, size_t operand_count,
					void *user_data)
{
	struct avrcp_browsing_header *pdu = (void *) operands;
	struct avrcp *session = user_data;
	struct avrcp_player *player = session->controller->player;
	struct media_player *mp = player->user_data;
	int ret;

	if (pdu == NULL) {
		ret = -ETIMEDOUT;
		goto done;
	}

	if (pdu->params[0] != AVRCP_STATUS_SUCCESS) {
		ret = -EINVAL;
		goto done;
	}

	ret = get_be32(&pdu->params[1]);

done:
	if (ret < 0) {
		g_free(player->change_path);
		player->change_path = NULL;
	} else {
		g_free(player->path);
		player->path = player->change_path;
		player->change_path = NULL;
	}

	media_player_change_folder_complete(mp, player->path,
						player->change_uid, ret);

	player->change_uid = 0;

	return FALSE;
}