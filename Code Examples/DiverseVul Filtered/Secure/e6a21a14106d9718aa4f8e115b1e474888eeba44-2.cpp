static void vidtv_s302m_access_unit_destroy(struct vidtv_encoder *e)
{
	struct vidtv_access_unit *head = e->access_units;
	struct vidtv_access_unit *tmp = NULL;

	while (head) {
		tmp = head;
		head = head->next;
		kfree(tmp);
	}

	e->access_units = NULL;
}