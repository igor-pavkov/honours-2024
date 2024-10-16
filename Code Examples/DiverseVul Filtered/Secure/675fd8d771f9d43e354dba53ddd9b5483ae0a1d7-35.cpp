static const char *shadow_copy2_find_snapdir(TALLOC_CTX *mem_ctx,
					     struct vfs_handle_struct *handle,
					     struct smb_filename *smb_fname)
{
	char *path, *p;
	const char *snapdir;
	struct shadow_copy2_config *config;

	SMB_VFS_HANDLE_GET_DATA(handle, config, struct shadow_copy2_config,
				return NULL);

	/*
	 * If the non-snapdisrseverywhere mode, we should not search!
	 */
	if (!config->snapdirseverywhere) {
		return config->snapshot_basepath;
	}

	path = talloc_asprintf(mem_ctx, "%s/%s",
			       handle->conn->connectpath,
			       smb_fname->base_name);
	if (path == NULL) {
		return NULL;
	}

	snapdir = have_snapdir(handle, path);
	if (snapdir != NULL) {
		TALLOC_FREE(path);
		return snapdir;
	}

	while ((p = strrchr(path, '/')) && (p > path)) {

		p[0] = '\0';

		snapdir = have_snapdir(handle, path);
		if (snapdir != NULL) {
			TALLOC_FREE(path);
			return snapdir;
		}
	}
	TALLOC_FREE(path);
	return NULL;
}