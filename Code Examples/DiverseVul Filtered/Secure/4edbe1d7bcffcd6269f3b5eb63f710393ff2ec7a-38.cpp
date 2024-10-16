static int validate_params(uint cmd, struct dm_ioctl *param)
{
	/* Always clear this flag */
	param->flags &= ~DM_BUFFER_FULL_FLAG;
	param->flags &= ~DM_UEVENT_GENERATED_FLAG;
	param->flags &= ~DM_SECURE_DATA_FLAG;
	param->flags &= ~DM_DATA_OUT_FLAG;

	/* Ignores parameters */
	if (cmd == DM_REMOVE_ALL_CMD ||
	    cmd == DM_LIST_DEVICES_CMD ||
	    cmd == DM_LIST_VERSIONS_CMD)
		return 0;

	if (cmd == DM_DEV_CREATE_CMD) {
		if (!*param->name) {
			DMWARN("name not supplied when creating device");
			return -EINVAL;
		}
	} else if (*param->uuid && *param->name) {
		DMWARN("only supply one of name or uuid, cmd(%u)", cmd);
		return -EINVAL;
	}

	/* Ensure strings are terminated */
	param->name[DM_NAME_LEN - 1] = '\0';
	param->uuid[DM_UUID_LEN - 1] = '\0';

	return 0;
}