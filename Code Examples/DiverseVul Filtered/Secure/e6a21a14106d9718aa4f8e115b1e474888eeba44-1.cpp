static void vidtv_s302m_alloc_au(struct vidtv_encoder *e)
{
	struct vidtv_access_unit *sync_au = NULL;
	struct vidtv_access_unit *temp = NULL;

	if (e->sync && e->sync->is_video_encoder) {
		sync_au = e->sync->access_units;

		while (sync_au) {
			temp = vidtv_s302m_access_unit_init(e->access_units);
			if (!e->access_units)
				e->access_units = temp;

			sync_au = sync_au->next;
		}

		return;
	}

	e->access_units = vidtv_s302m_access_unit_init(NULL);
}