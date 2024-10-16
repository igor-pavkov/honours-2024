static int ct_get_total_numberofitems(struct media_player *mp, const char *name,
						void *user_data)
{
	struct avrcp_player *player = user_data;
	struct avrcp *session;

	session = player->sessions->data;
	set_ct_player(session, player);

	if (session->controller->version != 0x0106) {
		error("version not supported");
		return -1;
	}

	if (g_str_has_prefix(name, "/NowPlaying"))
		player->scope = 0x03;
	else if (g_str_has_suffix(name, "/search"))
		player->scope = 0x02;
	else
		player->scope = 0x01;

	avrcp_get_total_numberofitems(session);

	return 0;
}