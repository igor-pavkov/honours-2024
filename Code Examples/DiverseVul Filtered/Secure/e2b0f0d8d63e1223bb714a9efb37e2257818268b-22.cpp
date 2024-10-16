static gboolean avrcp_list_items_rsp(struct avctp *conn, uint8_t *operands,
					size_t operand_count, void *user_data)
{
	struct avrcp_browsing_header *pdu = (void *) operands;
	struct avrcp *session = user_data;
	struct avrcp_player *player = session->controller->player;
	struct pending_list_items *p = player->p;
	uint16_t count;
	uint64_t items;
	size_t i;
	int err = 0;

	if (pdu == NULL) {
		err = -ETIMEDOUT;
		goto done;
	}

	/* AVRCP 1.5 - Page 76:
	 * If the TG receives a GetFolderItems command for an empty folder then
	 * the TG shall return the error (= Range Out of Bounds) in the status
	 * field of the GetFolderItems response.
	 */
	if (pdu->params[0] == AVRCP_STATUS_OUT_OF_BOUNDS)
		goto done;

	if (pdu->params[0] != AVRCP_STATUS_SUCCESS || operand_count < 5) {
		err = -EINVAL;
		goto done;
	}

	count = get_be16(&operands[6]);
	if (count == 0)
		goto done;

	for (i = 8; count && i + 3 < operand_count; count--) {
		struct media_item *item;
		uint8_t type;
		uint16_t len;

		type = operands[i++];
		len = get_be16(&operands[i]);
		i += 2;

		if (type != 0x03 && type != 0x02) {
			i += len;
			continue;
		}

		if (i + len > operand_count) {
			error("Invalid item length");
			break;
		}

		if (type == 0x03)
			item = parse_media_element(session, &operands[i], len);
		else
			item = parse_media_folder(session, &operands[i], len);

		if (item)
			p->items = g_slist_append(p->items, item);

		i += len;
	}

	items = g_slist_length(p->items);

	DBG("start %u end %u items %" PRIu64 " total %" PRIu64 "", p->start,
						p->end, items, p->total);

	if (items < p->total) {
		avrcp_list_items(session, p->start + items, p->end);
		return FALSE;
	}

done:
	media_player_list_complete(player->user_data, p->items, err);

	g_slist_free(p->items);
	g_free(p);
	player->p = NULL;

	return FALSE;
}