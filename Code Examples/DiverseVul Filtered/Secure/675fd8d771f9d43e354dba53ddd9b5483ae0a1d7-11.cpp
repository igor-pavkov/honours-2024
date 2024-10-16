static int shadow_copy2_connect(struct vfs_handle_struct *handle,
				const char *service, const char *user)
{
	struct shadow_copy2_config *config;
	int ret;
	const char *snapdir;
	const char *gmt_format;
	const char *sort_order;
	const char *basedir;
	const char *mount_point;

	DEBUG(10, (__location__ ": cnum[%u], connectpath[%s]\n",
		   (unsigned)handle->conn->cnum,
		   handle->conn->connectpath));

	ret = SMB_VFS_NEXT_CONNECT(handle, service, user);
	if (ret < 0) {
		return ret;
	}

	config = talloc_zero(handle->conn, struct shadow_copy2_config);
	if (config == NULL) {
		DEBUG(0, ("talloc_zero() failed\n"));
		errno = ENOMEM;
		return -1;
	}

	gmt_format = lp_parm_const_string(SNUM(handle->conn),
					  "shadow", "format",
					  GMT_FORMAT);
	config->gmt_format = talloc_strdup(config, gmt_format);
	if (config->gmt_format == NULL) {
		DEBUG(0, ("talloc_strdup() failed\n"));
		errno = ENOMEM;
		return -1;
	}

	config->use_sscanf = lp_parm_bool(SNUM(handle->conn),
					  "shadow", "sscanf", false);

	config->use_localtime = lp_parm_bool(SNUM(handle->conn),
					     "shadow", "localtime",
					     false);

	snapdir = lp_parm_const_string(SNUM(handle->conn),
				       "shadow", "snapdir",
				       ".snapshots");
	config->snapdir = talloc_strdup(config, snapdir);
	if (config->snapdir == NULL) {
		DEBUG(0, ("talloc_strdup() failed\n"));
		errno = ENOMEM;
		return -1;
	}

	config->snapdirseverywhere = lp_parm_bool(SNUM(handle->conn),
						  "shadow",
						  "snapdirseverywhere",
						  false);

	config->crossmountpoints = lp_parm_bool(SNUM(handle->conn),
						"shadow", "crossmountpoints",
						false);

	config->fixinodes = lp_parm_bool(SNUM(handle->conn),
					 "shadow", "fixinodes",
					 false);

	sort_order = lp_parm_const_string(SNUM(handle->conn),
					  "shadow", "sort", "desc");
	config->sort_order = talloc_strdup(config, sort_order);
	if (config->sort_order == NULL) {
		DEBUG(0, ("talloc_strdup() failed\n"));
		errno = ENOMEM;
		return -1;
	}

	mount_point = lp_parm_const_string(SNUM(handle->conn),
					   "shadow", "mountpoint", NULL);
	if (mount_point != NULL) {
		if (mount_point[0] != '/') {
			DEBUG(1, (__location__ " Warning: 'mountpoint' is "
				  "relative ('%s'), but it has to be an "
				  "absolute path. Ignoring provided value.\n",
				  mount_point));
			mount_point = NULL;
		} else {
			char *p;
			p = strstr(handle->conn->connectpath, mount_point);
			if (p != handle->conn->connectpath) {
				DEBUG(1, ("Warning: mount_point (%s) is not a "
					  "subdirectory of the share root "
					  "(%s). Ignoring provided value.\n",
					  mount_point,
					  handle->conn->connectpath));
				mount_point = NULL;
			}
		}
	}

	if (mount_point != NULL) {
		config->mount_point = talloc_strdup(config, mount_point);
		if (config->mount_point == NULL) {
			DEBUG(0, (__location__ " talloc_strdup() failed\n"));
			return -1;
		}
	} else {
		config->mount_point = shadow_copy2_find_mount_point(config,
								    handle);
		if (config->mount_point == NULL) {
			DBG_WARNING("shadow_copy2_find_mount_point "
				    "of the share root '%s' failed: %s\n",
				    handle->conn->connectpath, strerror(errno));
			return -1;
		}
	}

	basedir = lp_parm_const_string(SNUM(handle->conn),
				       "shadow", "basedir", NULL);

	if (basedir != NULL) {
		if (basedir[0] != '/') {
			DEBUG(1, (__location__ " Warning: 'basedir' is "
				  "relative ('%s'), but it has to be an "
				  "absolute path. Disabling basedir.\n",
				  basedir));
		} else {
			char *p;
			p = strstr(basedir, config->mount_point);
			if (p != basedir) {
				DEBUG(1, ("Warning: basedir (%s) is not a "
					  "subdirectory of the share root's "
					  "mount point (%s). "
					  "Disabling basedir\n",
					  basedir, config->mount_point));
			} else {
				config->basedir = talloc_strdup(config,
								basedir);
				if (config->basedir == NULL) {
					DEBUG(0, ("talloc_strdup() failed\n"));
					errno = ENOMEM;
					return -1;
				}
			}
		}
	}

	if (config->snapdirseverywhere && config->basedir != NULL) {
		DEBUG(1, (__location__ " Warning: 'basedir' is incompatible "
			  "with 'snapdirseverywhere'. Disabling basedir.\n"));
		TALLOC_FREE(config->basedir);
	}

	if (config->crossmountpoints && config->basedir != NULL) {
		DEBUG(1, (__location__ " Warning: 'basedir' is incompatible "
			  "with 'crossmountpoints'. Disabling basedir.\n"));
		TALLOC_FREE(config->basedir);
	}

	if (config->basedir == NULL) {
		config->basedir = config->mount_point;
	}

	if (strlen(config->basedir) != strlen(handle->conn->connectpath)) {
		config->rel_connectpath = talloc_strdup(config,
			handle->conn->connectpath + strlen(config->basedir));
		if (config->rel_connectpath == NULL) {
			DEBUG(0, ("talloc_strdup() failed\n"));
			errno = ENOMEM;
			return -1;
		}
	}

	if (config->snapdir[0] == '/') {
		config->snapdir_absolute = true;

		if (config->snapdirseverywhere == true) {
			DEBUG(1, (__location__ " Warning: An absolute snapdir "
				  "is incompatible with 'snapdirseverywhere', "
				  "setting 'snapdirseverywhere' to false.\n"));
			config->snapdirseverywhere = false;
		}

		if (config->crossmountpoints == true) {
			DEBUG(1, (__location__ " Warning: 'crossmountpoints' "
				  "is not supported with an absolute snapdir. "
				  "Disabling it.\n"));
			config->crossmountpoints = false;
		}

		config->snapshot_basepath = config->snapdir;
	} else {
		config->snapshot_basepath = talloc_asprintf(config, "%s/%s",
				config->mount_point, config->snapdir);
		if (config->snapshot_basepath == NULL) {
			DEBUG(0, ("talloc_asprintf() failed\n"));
			errno = ENOMEM;
			return -1;
		}
	}

	DEBUG(10, ("shadow_copy2_connect: configuration:\n"
		   "  share root: '%s'\n"
		   "  basedir: '%s'\n"
		   "  mountpoint: '%s'\n"
		   "  rel share root: '%s'\n"
		   "  snapdir: '%s'\n"
		   "  snapshot base path: '%s'\n"
		   "  format: '%s'\n"
		   "  use sscanf: %s\n"
		   "  snapdirs everywhere: %s\n"
		   "  cross mountpoints: %s\n"
		   "  fix inodes: %s\n"
		   "  sort order: %s\n"
		   "",
		   handle->conn->connectpath,
		   config->basedir,
		   config->mount_point,
		   config->rel_connectpath,
		   config->snapdir,
		   config->snapshot_basepath,
		   config->gmt_format,
		   config->use_sscanf ? "yes" : "no",
		   config->snapdirseverywhere ? "yes" : "no",
		   config->crossmountpoints ? "yes" : "no",
		   config->fixinodes ? "yes" : "no",
		   config->sort_order
		   ));


	SMB_VFS_HANDLE_SET_DATA(handle, config,
				NULL, struct shadow_copy2_config,
				return -1);

	return 0;
}