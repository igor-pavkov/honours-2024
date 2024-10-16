static int ct_search(struct media_player *mp, const char *string,
							void *user_data)
{
	struct avrcp_player *player = user_data;
	struct avrcp *session;

	session = player->sessions->data;

	set_ct_player(session, player);
	avrcp_search(session, string);

	return 0;
}