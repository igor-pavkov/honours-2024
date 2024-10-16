static struct media_item *parse_media_folder(struct avrcp *session,
					uint8_t *operands, uint16_t len)
{
	struct avrcp_player *player = session->controller->player;
	struct media_player *mp = player->user_data;
	struct media_item *item;
	uint16_t namelen;
	char name[255];
	uint64_t uid;
	uint8_t type;
	uint8_t playable;

	if (len < 12)
		return NULL;

	uid = get_be64(&operands[0]);
	type = operands[8];
	playable = operands[9];

	namelen = MIN(get_be16(&operands[12]), sizeof(name) - 1);
	if (namelen > 0) {
		memcpy(name, &operands[14], namelen);
		name[namelen] = '\0';
	}

	item = media_player_create_folder(mp, name, type, uid);
	if (!item)
		return NULL;

	media_item_set_playable(item, playable & 0x01);

	return item;
}