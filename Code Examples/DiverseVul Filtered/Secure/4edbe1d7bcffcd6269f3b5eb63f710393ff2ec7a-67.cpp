static struct hash_cell *alloc_cell(const char *name, const char *uuid,
				    struct mapped_device *md)
{
	struct hash_cell *hc;

	hc = kmalloc(sizeof(*hc), GFP_KERNEL);
	if (!hc)
		return NULL;

	hc->name = kstrdup(name, GFP_KERNEL);
	if (!hc->name) {
		kfree(hc);
		return NULL;
	}

	if (!uuid)
		hc->uuid = NULL;

	else {
		hc->uuid = kstrdup(uuid, GFP_KERNEL);
		if (!hc->uuid) {
			kfree(hc->name);
			kfree(hc);
			return NULL;
		}
	}

	INIT_LIST_HEAD(&hc->name_list);
	INIT_LIST_HEAD(&hc->uuid_list);
	hc->md = md;
	hc->new_map = NULL;
	return hc;
}