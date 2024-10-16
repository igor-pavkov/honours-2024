static void avrcp_parse_attribute_list(struct avrcp_player *player,
					uint8_t *operands, uint8_t count)
{
	struct media_player *mp = player->user_data;
	struct media_item *item;
	int i;

	item = media_player_set_playlist_item(mp, player->uid);

	for (i = 0; count > 0; count--) {
		uint32_t id;
		uint16_t charset, len;

		id = get_be32(&operands[i]);
		i += sizeof(uint32_t);

		charset = get_be16(&operands[i]);
		i += sizeof(uint16_t);

		len = get_be16(&operands[i]);
		i += sizeof(uint16_t);

		if (charset == 106) {
			const char *key = metadata_to_str(id);

			if (key != NULL)
				media_player_set_metadata(mp, item,
							metadata_to_str(id),
							&operands[i], len);
		}

		i += len;
	}
}