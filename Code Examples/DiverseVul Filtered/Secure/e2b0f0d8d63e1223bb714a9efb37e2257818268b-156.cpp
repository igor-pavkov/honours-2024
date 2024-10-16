static int ct_list_items(struct media_player *mp, const char *name,
				uint32_t start, uint32_t end, void *user_data)
{
	struct avrcp_player *player = user_data;
	struct avrcp *session;
	struct pending_list_items *p;

	if (player->p != NULL)
		return -EBUSY;

	session = player->sessions->data;

	set_ct_player(session, player);

	if (g_str_has_prefix(name, "/NowPlaying"))
		player->scope = 0x03;
	else if (g_str_has_suffix(name, "/search"))
		player->scope = 0x02;
	else
		player->scope = 0x01;

	avrcp_list_items(session, start, end);

	p = g_new0(struct pending_list_items, 1);
	p->start = start;
	p->end = end;
	p->total = (uint64_t) (p->end - p->start) + 1;
	player->p = p;

	return 0;
}