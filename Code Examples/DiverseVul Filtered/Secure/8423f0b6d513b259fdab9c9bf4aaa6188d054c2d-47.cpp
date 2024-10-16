static const char *strip_task_path(const char *path)
{
	const char *ptr, *ptrl = NULL;
	for (ptr = path; *ptr; ptr++) {
		if (*ptr == '/')
			ptrl = ptr + 1;
	}
	return ptrl;
}