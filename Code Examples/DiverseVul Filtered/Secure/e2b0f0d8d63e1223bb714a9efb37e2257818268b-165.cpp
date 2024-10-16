static gboolean avrcp_set_browsed_player_rsp(struct avctp *conn,
						uint8_t *operands,
						size_t operand_count,
						void *user_data)
{
	struct avrcp *session = user_data;
	struct avrcp_player *player = session->controller->player;
	struct media_player *mp = player->user_data;
	struct avrcp_browsing_header *pdu = (void *) operands;
	uint32_t items;
	char **folders;
	uint8_t depth, count;
	size_t i;

	if (pdu == NULL || pdu->params[0] != AVRCP_STATUS_SUCCESS ||
							operand_count < 13)
		return FALSE;

	player->uid_counter = get_be16(&pdu->params[1]);
	player->browsed = true;

	items = get_be32(&pdu->params[3]);

	depth = pdu->params[9];

	folders = g_new0(char *, depth + 2);
	folders[0] = g_strdup("/Filesystem");

	for (i = 10, count = 1; count - 1 < depth && i < operand_count;
								count++) {
		uint8_t len;

		len = pdu->params[i++];
		if (!len)
			continue;

		if (i + len > operand_count) {
			error("Invalid folder length");
			break;
		}

		folders[count] = g_memdup(&pdu->params[i], len);
		i += len;
	}

	player->path = g_build_pathv("/", folders);
	g_strfreev(folders);

	media_player_set_folder(mp, player->path, items);

	return FALSE;
}