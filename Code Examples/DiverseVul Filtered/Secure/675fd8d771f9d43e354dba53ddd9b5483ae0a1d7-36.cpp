static char *shadow_copy2_convert(TALLOC_CTX *mem_ctx,
				  struct vfs_handle_struct *handle,
				  const char *name, time_t timestamp)
{
	struct smb_filename converted_fname;
	char *result = NULL;
	size_t *slashes = NULL;
	unsigned num_slashes;
	char *path = NULL;
	size_t pathlen;
	char *insert = NULL;
	char *converted = NULL;
	size_t insertlen;
	int i, saved_errno;
	size_t min_offset;
	struct shadow_copy2_config *config;

	SMB_VFS_HANDLE_GET_DATA(handle, config, struct shadow_copy2_config,
				return NULL);

	DEBUG(10, ("converting '%s'\n", name));

	if (!config->snapdirseverywhere) {
		int ret;
		char *snapshot_path;

		snapshot_path = shadow_copy2_snapshot_path(talloc_tos(),
							   handle,
							   timestamp);
		if (snapshot_path == NULL) {
			goto fail;
		}

		if (config->rel_connectpath == NULL) {
			converted = talloc_asprintf(mem_ctx, "%s/%s",
						    snapshot_path, name);
		} else {
			converted = talloc_asprintf(mem_ctx, "%s/%s/%s",
						    snapshot_path,
						    config->rel_connectpath,
						    name);
		}
		if (converted == NULL) {
			goto fail;
		}

		ZERO_STRUCT(converted_fname);
		converted_fname.base_name = converted;

		ret = SMB_VFS_NEXT_LSTAT(handle, &converted_fname);
		DEBUG(10, ("Trying[not snapdirseverywhere] %s: %d (%s)\n",
			   converted,
			   ret, ret == 0 ? "ok" : strerror(errno)));
		if (ret == 0) {
			DEBUG(10, ("Found %s\n", converted));
			result = converted;
			converted = NULL;
			goto fail;
		} else {
			errno = ENOENT;
			goto fail;
		}
		/* never reached ... */
	}

	if (name[0] == 0) {
		path = talloc_strdup(mem_ctx, handle->conn->connectpath);
	} else {
		path = talloc_asprintf(
			mem_ctx, "%s/%s", handle->conn->connectpath, name);
	}
	if (path == NULL) {
		errno = ENOMEM;
		goto fail;
	}
	pathlen = talloc_get_size(path)-1;

	if (!shadow_copy2_find_slashes(talloc_tos(), path,
				       &slashes, &num_slashes)) {
		goto fail;
	}

	insert = shadow_copy2_insert_string(talloc_tos(), handle, timestamp);
	if (insert == NULL) {
		goto fail;
	}
	insertlen = talloc_get_size(insert)-1;

	/*
	 * Note: We deliberatly don't expensively initialize the
	 * array with talloc_zero here: Putting zero into
	 * converted[pathlen+insertlen] below is sufficient, because
	 * in the following for loop, the insert string is inserted
	 * at various slash places. So the memory up to position
	 * pathlen+insertlen will always be initialized when the
	 * converted string is used.
	 */
	converted = talloc_array(mem_ctx, char, pathlen + insertlen + 1);
	if (converted == NULL) {
		goto fail;
	}

	if (path[pathlen-1] != '/') {
		/*
		 * Append a fake slash to find the snapshot root
		 */
		size_t *tmp;
		tmp = talloc_realloc(talloc_tos(), slashes,
				     size_t, num_slashes+1);
		if (tmp == NULL) {
			goto fail;
		}
		slashes = tmp;
		slashes[num_slashes] = pathlen;
		num_slashes += 1;
	}

	min_offset = 0;

	if (!config->crossmountpoints) {
		min_offset = strlen(config->mount_point);
	}

	memcpy(converted, path, pathlen+1);
	converted[pathlen+insertlen] = '\0';

	ZERO_STRUCT(converted_fname);
	converted_fname.base_name = converted;

	for (i = num_slashes-1; i>=0; i--) {
		int ret;
		size_t offset;

		offset = slashes[i];

		if (offset < min_offset) {
			errno = ENOENT;
			goto fail;
		}

		memcpy(converted+offset, insert, insertlen);

		offset += insertlen;
		memcpy(converted+offset, path + slashes[i],
		       pathlen - slashes[i]);

		ret = SMB_VFS_NEXT_LSTAT(handle, &converted_fname);

		DEBUG(10, ("Trying[snapdirseverywhere] %s: %d (%s)\n",
			   converted,
			   ret, ret == 0 ? "ok" : strerror(errno)));
		if (ret == 0) {
			/* success */
			break;
		}
		if (errno == ENOTDIR) {
			/*
			 * This is a valid condition: We appended the
			 * .snaphots/@GMT.. to a file name. Just try
			 * with the upper levels.
			 */
			continue;
		}
		if (errno != ENOENT) {
			/* Other problem than "not found" */
			goto fail;
		}
	}

	if (i >= 0) {
		/*
		 * Found something
		 */
		DEBUG(10, ("Found %s\n", converted));
		result = converted;
		converted = NULL;
	} else {
		errno = ENOENT;
	}
fail:
	saved_errno = errno;
	TALLOC_FREE(converted);
	TALLOC_FREE(insert);
	TALLOC_FREE(slashes);
	TALLOC_FREE(path);
	errno = saved_errno;
	return result;
}