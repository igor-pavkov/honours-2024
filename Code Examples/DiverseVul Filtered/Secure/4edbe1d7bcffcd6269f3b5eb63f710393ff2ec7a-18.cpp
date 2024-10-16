static struct hash_cell *__get_name_cell(const char *str)
{
	struct hash_cell *hc;
	unsigned int h = hash_str(str);

	list_for_each_entry (hc, _name_buckets + h, name_list)
		if (!strcmp(hc->name, str)) {
			dm_get(hc->md);
			return hc;
		}

	return NULL;
}