static int sqfs_split_path(char **file, char **dir, const char *path)
{
	char *dirc, *basec, *bname, *dname, *tmp_path;
	int ret = 0;

	*file = NULL;
	*dir = NULL;
	dirc = NULL;
	basec = NULL;
	bname = NULL;
	dname = NULL;
	tmp_path = NULL;

	/* check for first slash in path*/
	if (path[0] == '/') {
		tmp_path = strdup(path);
		if (!tmp_path) {
			ret = -ENOMEM;
			goto out;
		}
	} else {
		tmp_path = malloc(strlen(path) + 2);
		if (!tmp_path) {
			ret = -ENOMEM;
			goto out;
		}
		tmp_path[0] = '/';
		strcpy(tmp_path + 1, path);
	}

	/* String duplicates */
	dirc = strdup(tmp_path);
	if (!dirc) {
		ret = -ENOMEM;
		goto out;
	}

	basec = strdup(tmp_path);
	if (!basec) {
		ret = -ENOMEM;
		goto out;
	}

	dname = sqfs_dirname(dirc);
	bname = sqfs_basename(basec);

	*file = strdup(bname);

	if (!*file) {
		ret = -ENOMEM;
		goto out;
	}

	if (*dname == '\0') {
		*dir = malloc(2);
		if (!*dir) {
			ret = -ENOMEM;
			goto out;
		}

		(*dir)[0] = '/';
		(*dir)[1] = '\0';
	} else {
		*dir = strdup(dname);
		if (!*dir) {
			ret = -ENOMEM;
			goto out;
		}
	}

out:
	if (ret) {
		free(*file);
		free(*dir);
		*dir = NULL;
		*file = NULL;
	}
	free(basec);
	free(dirc);
	free(tmp_path);

	return ret;
}