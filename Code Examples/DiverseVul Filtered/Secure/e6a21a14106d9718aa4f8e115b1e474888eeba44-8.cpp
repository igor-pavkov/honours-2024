static struct vidtv_access_unit *vidtv_s302m_access_unit_init(struct vidtv_access_unit *head)
{
	struct vidtv_access_unit *au;

	au = kzalloc(sizeof(*au), GFP_KERNEL);
	if (!au)
		return NULL;

	if (head) {
		while (head->next)
			head = head->next;

		head->next = au;
	}

	return au;
}