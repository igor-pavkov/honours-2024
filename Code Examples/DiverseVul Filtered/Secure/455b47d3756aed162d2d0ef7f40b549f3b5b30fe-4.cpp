char *compose_abspath(ctrl_t *ctrl, char *path)
{
	char *ptr;
	char cwd[sizeof(ctrl->cwd)];

	if (path && path[0] == '/') {
		strlcpy(cwd, ctrl->cwd, sizeof(cwd));
		memset(ctrl->cwd, 0, sizeof(ctrl->cwd));
	}

	ptr = compose_path(ctrl, path);

	if (path && path[0] == '/')
		strlcpy(ctrl->cwd, cwd, sizeof(ctrl->cwd));

	return ptr;
}