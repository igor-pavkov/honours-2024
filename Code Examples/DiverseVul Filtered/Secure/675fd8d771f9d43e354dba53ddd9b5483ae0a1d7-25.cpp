static bool shadow_copy2_strip_snapshot(TALLOC_CTX *mem_ctx,
					struct vfs_handle_struct *handle,
					const char *name,
					time_t *ptimestamp,
					char **pstripped)
{
	struct tm tm;
	time_t timestamp;
	const char *p;
	char *q;
	char *stripped;
	size_t rest_len, dst_len;
	struct shadow_copy2_config *config;
	const char *snapdir;
	ssize_t snapdirlen;
	ptrdiff_t len_before_gmt;

	SMB_VFS_HANDLE_GET_DATA(handle, config, struct shadow_copy2_config,
				return false);

	DEBUG(10, (__location__ ": enter path '%s'\n", name));

	p = strstr_m(name, "@GMT-");
	if (p == NULL) {
		DEBUG(11, ("@GMT not found\n"));
		goto no_snapshot;
	}
	if ((p > name) && (p[-1] != '/')) {
		/* the GMT-token does not start a path-component */
		DEBUG(10, ("not at start, p=%p, name=%p, p[-1]=%d\n",
			   p, name, (int)p[-1]));
		goto no_snapshot;
	}

	/*
	 * Figure out whether we got an already converted string. One
	 * case where this happens is in a smb2 create call with the
	 * mxac create blob set. We do the get_acl call on
	 * fsp->fsp_name, which is already converted. We are converted
	 * if we got a file name of the form ".snapshots/@GMT-",
	 * i.e. ".snapshots/" precedes "p".
	 */

	snapdir = lp_parm_const_string(SNUM(handle->conn), "shadow", "snapdir",
				       ".snapshots");
	snapdirlen = strlen(snapdir);
	len_before_gmt = p - name;

	if ((len_before_gmt >= (snapdirlen + 1)) && (p[-1] == '/')) {
		const char *parent_snapdir = p - (snapdirlen+1);

		DEBUG(10, ("parent_snapdir = %s\n", parent_snapdir));

		if (strncmp(parent_snapdir, snapdir, snapdirlen) == 0) {
			DEBUG(10, ("name=%s is already converted\n", name));
			goto no_snapshot;
		}
	}
	q = strptime(p, GMT_FORMAT, &tm);
	if (q == NULL) {
		DEBUG(10, ("strptime failed\n"));
		goto no_snapshot;
	}
	tm.tm_isdst = -1;
	timestamp = timegm(&tm);
	if (timestamp == (time_t)-1) {
		DEBUG(10, ("timestamp==-1\n"));
		goto no_snapshot;
	}
	if (q[0] == '\0') {
		/*
		 * The name consists of only the GMT token or the GMT
		 * token is at the end of the path. XP seems to send
		 * @GMT- at the end under certain circumstances even
		 * with a path prefix.
		 */
		if (pstripped != NULL) {
			stripped = talloc_strndup(mem_ctx, name, p - name);
			if (stripped == NULL) {
				return false;
			}
			*pstripped = stripped;
		}
		*ptimestamp = timestamp;
		return true;
	}
	if (q[0] != '/') {
		/*
		 * It is not a complete path component, i.e. the path
		 * component continues after the gmt-token.
		 */
		DEBUG(10, ("q[0] = %d\n", (int)q[0]));
		goto no_snapshot;
	}
	q += 1;

	rest_len = strlen(q);
	dst_len = (p-name) + rest_len;

	if (config->snapdirseverywhere) {
		char *insert;
		bool have_insert;
		insert = shadow_copy2_insert_string(talloc_tos(), handle,
						    timestamp);
		if (insert == NULL) {
			errno = ENOMEM;
			return false;
		}

		DEBUG(10, (__location__ ": snapdirseverywhere mode.\n"
			   "path '%s'.\n"
			   "insert string '%s'\n", name, insert));

		have_insert = (strstr(name, insert+1) != NULL);
		DEBUG(10, ("have_insert=%d, name=%s, insert+1=%s\n",
			   (int)have_insert, name, insert+1));
		if (have_insert) {
			DEBUG(10, (__location__ ": insert string '%s' found in "
				   "path '%s' found in snapdirseverywhere mode "
				   "==> already converted\n", insert, name));
			TALLOC_FREE(insert);
			goto no_snapshot;
		}
		TALLOC_FREE(insert);
	} else {
		char *snapshot_path;
		char *s;

		snapshot_path = shadow_copy2_snapshot_path(talloc_tos(),
							   handle,
							   timestamp);
		if (snapshot_path == NULL) {
			errno = ENOMEM;
			return false;
		}

		DEBUG(10, (__location__ " path: '%s'.\n"
			   "snapshot path: '%s'\n", name, snapshot_path));

		s = strstr(name, snapshot_path);
		if (s == name) {
			/*
			 * this starts with "snapshot_basepath/GMT-Token"
			 * so it is already a converted absolute
			 * path. Don't process further.
			 */
			DEBUG(10, (__location__ ": path '%s' starts with "
				   "snapshot path '%s' (not in "
				   "snapdirseverywhere mode) ==> "
				   "already converted\n", name, snapshot_path));
			talloc_free(snapshot_path);
			goto no_snapshot;
		}
		talloc_free(snapshot_path);
	}

	if (pstripped != NULL) {
		stripped = talloc_array(mem_ctx, char, dst_len+1);
		if (stripped == NULL) {
			errno = ENOMEM;
			return false;
		}
		if (p > name) {
			memcpy(stripped, name, p-name);
		}
		if (rest_len > 0) {
			memcpy(stripped + (p-name), q, rest_len);
		}
		stripped[dst_len] = '\0';
		*pstripped = stripped;
	}
	*ptimestamp = timestamp;
	return true;
no_snapshot:
	*ptimestamp = 0;
	return true;
}