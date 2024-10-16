void smbd_notify_cancel_by_smbreq(const struct smb_request *smbreq)
{
	struct smbd_server_connection *sconn = smbreq->sconn;
	struct notify_mid_map *map;

	for (map = sconn->smb1.notify_mid_maps; map; map = map->next) {
		if (map->req->req == smbreq) {
			break;
		}
	}

	if (map == NULL) {
		return;
	}

	smbd_notify_cancel_by_map(map);
}