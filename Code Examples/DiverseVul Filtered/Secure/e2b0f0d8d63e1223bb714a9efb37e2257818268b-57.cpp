static int ct_play_item(struct media_player *mp, const char *name,
						uint64_t uid, void *user_data)
{
	struct avrcp_player *player = user_data;
	struct avrcp *session;

	if (player->p != NULL)
		return -EBUSY;

	session = player->sessions->data;
	set_ct_player(session, player);

	if (g_strrstr(name, "/NowPlaying"))
		player->scope = 0x03;
	else if (g_strrstr(name, "/Search"))
		player->scope = 0x02;
	else
		player->scope = 0x01;

	avrcp_play_item(session, uid);

	return 0;
}