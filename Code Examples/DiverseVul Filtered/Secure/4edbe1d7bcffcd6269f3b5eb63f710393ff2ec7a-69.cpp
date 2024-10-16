static struct hash_cell *__get_uuid_cell(const char *str)
{
	struct hash_cell *hc;
	unsigned int h = hash_str(str);

	list_for_each_entry (hc, _uuid_buckets + h, uuid_list)
		if (!strcmp(hc->uuid, str)) {
			dm_get(hc->md);
			return hc;
		}

	return NULL;
}