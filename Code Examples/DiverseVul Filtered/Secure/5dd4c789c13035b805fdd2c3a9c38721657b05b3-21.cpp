bool remove_pending_change_notify_requests_by_mid(
	struct smbd_server_connection *sconn, uint64_t mid)
{
	struct notify_mid_map *map;

	for (map = sconn->smb1.notify_mid_maps; map; map = map->next) {
		if (map->mid == mid) {
			break;
		}
	}

	if (map == NULL) {
		return false;
	}

	smbd_notify_cancel_by_map(map);
	return true;
}