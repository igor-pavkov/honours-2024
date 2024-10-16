static struct hash_cell *__find_device_hash_cell(struct dm_ioctl *param)
{
	struct hash_cell *hc = NULL;

	if (*param->uuid) {
		if (*param->name || param->dev)
			return NULL;

		hc = __get_uuid_cell(param->uuid);
		if (!hc)
			return NULL;
	} else if (*param->name) {
		if (param->dev)
			return NULL;

		hc = __get_name_cell(param->name);
		if (!hc)
			return NULL;
	} else if (param->dev) {
		hc = __get_dev_cell(param->dev);
		if (!hc)
			return NULL;
	} else
		return NULL;

	/*
	 * Sneakily write in both the name and the uuid
	 * while we have the cell.
	 */
	strlcpy(param->name, hc->name, sizeof(param->name));
	if (hc->uuid)
		strlcpy(param->uuid, hc->uuid, sizeof(param->uuid));
	else
		param->uuid[0] = '\0';

	if (hc->new_map)
		param->flags |= DM_INACTIVE_PRESENT_FLAG;
	else
		param->flags &= ~DM_INACTIVE_PRESENT_FLAG;

	return hc;
}