static struct media_item *parse_media_element(struct avrcp *session,
					uint8_t *operands, uint16_t len)
{
	struct avrcp_player *player;
	struct media_player *mp;
	struct media_item *item;
	uint16_t namelen;
	char name[255];
	uint64_t uid;

	if (len < 13)
		return NULL;

	uid = get_be64(&operands[0]);

	namelen = MIN(get_be16(&operands[11]), sizeof(name) - 1);
	if (namelen > 0) {
		memcpy(name, &operands[13], namelen);
		name[namelen] = '\0';
	}

	player = session->controller->player;
	mp = player->user_data;

	item = media_player_create_item(mp, name, PLAYER_ITEM_TYPE_AUDIO, uid);
	if (item == NULL)
		return NULL;

	media_item_set_playable(item, true);

	return item;
}