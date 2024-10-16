static int ct_change_folder(struct media_player *mp, const char *path,
					uint64_t uid, void *user_data)
{
	struct avrcp_player *player = user_data;
	struct avrcp *session;
	uint8_t direction;

	session = player->sessions->data;
	set_ct_player(session, player);
	player->change_path = g_strdup(path);
	player->change_uid = uid;

	direction = g_str_has_prefix(path, player->path) ? 0x01 : 0x00;

	avrcp_change_path(session, direction, uid);

	return 0;
}