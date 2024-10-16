char *make_traverse_path(char *path, const struct traverse_info *info, const struct name_entry *n)
{
	int len = tree_entry_len(n);
	int pathlen = info->pathlen;

	path[pathlen + len] = 0;
	for (;;) {
		memcpy(path + pathlen, n->path, len);
		if (!pathlen)
			break;
		path[--pathlen] = '/';
		n = &info->name;
		len = tree_entry_len(n);
		info = info->prev;
		pathlen -= len;
	}
	return path;
}