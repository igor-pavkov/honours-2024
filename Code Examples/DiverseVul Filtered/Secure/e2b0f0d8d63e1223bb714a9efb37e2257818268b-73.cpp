static GList *player_fill_media_attribute(struct avrcp_player *player,
					GList *attr_ids, uint8_t *buf,
					uint16_t *pos, uint16_t *offset)
{
	struct media_attribute_header {
		uint32_t id;
		uint16_t charset;
		uint16_t len;
	} *hdr = NULL;
	GList *l;

	for (l = attr_ids; l != NULL; l = g_list_delete_link(l, l)) {
		uint32_t attr = GPOINTER_TO_UINT(l->data);
		uint16_t attr_len;

		if (*offset == 0) {
			if (*pos + sizeof(*hdr) >= AVRCP_PDU_MTU)
				break;

			hdr = (void *) &buf[*pos];
			hdr->id = htonl(attr);
			/* Always use UTF-8 */
			hdr->charset = htons(AVRCP_CHARSET_UTF8);
			*pos += sizeof(*hdr);
		}

		attr_len = player_write_media_attribute(player, attr, buf,
								pos, offset);

		if (hdr != NULL)
			hdr->len = htons(attr_len);

		if (*offset > 0)
			break;
	}

	return l;
}