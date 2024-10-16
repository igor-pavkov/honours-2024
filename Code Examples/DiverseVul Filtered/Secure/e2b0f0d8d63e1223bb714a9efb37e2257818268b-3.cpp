static int ct_add_to_nowplaying(struct media_player *mp, const char *name,
						uint64_t uid, void *user_data)
{
	struct avrcp_player *player = user_data;
	struct avrcp *session;

	if (player->p != NULL)
		return -EBUSY;

	session = player->sessions->data;

	if (g_strrstr(name, "/NowPlaying"))
		player->scope = 0x03;
	else
		player->scope = 0x01;

	set_ct_player(session, player);
	avrcp_add_to_nowplaying(session, uid);

	return 0;
}