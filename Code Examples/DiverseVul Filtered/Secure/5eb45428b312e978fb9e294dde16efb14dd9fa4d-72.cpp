static bool cull_entries(int fd, char *name, char *net_type, char *net_link,
			 char *net_dev, bool *found_nicname)
{
	int i, ret;
	char *buf, *buf_end, *buf_start;
	struct stat sb;
	int n = 0;
	bool found, keep;
	struct entry_line *entry_lines = NULL;

	ret = fstat(fd, &sb);
	if (ret < 0) {
		usernic_error("Failed to fstat: %s\n", strerror(errno));
		return false;
	}

	if (!sb.st_size)
		return false;

	buf = lxc_strmmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (buf == MAP_FAILED) {
		usernic_error("Failed to establish shared memory mapping: %s\n",
			      strerror(errno));
		return false;
	}

	buf_start = buf;
	buf_end = buf + sb.st_size;
	while ((buf_start = find_line(buf_start, buf_end, name, net_type,
				      net_link, net_dev, &(bool){true}, &found,
				      &keep))) {
		struct entry_line *newe;

		newe = realloc(entry_lines, sizeof(*entry_lines) * (n + 1));
		if (!newe) {
			free(entry_lines);
			lxc_strmunmap(buf, sb.st_size);
			return false;
		}

		if (found)
			*found_nicname = true;

		entry_lines = newe;
		entry_lines[n].start = buf_start;
		entry_lines[n].len = get_eol(buf_start, buf_end) - entry_lines[n].start;
		entry_lines[n].keep = keep;
		n++;

		buf_start += entry_lines[n - 1].len + 1;
		if (buf_start >= buf_end)
			break;
	}

	buf_start = buf;
	for (i = 0; i < n; i++) {
		if (!entry_lines[i].keep)
			continue;

		memcpy(buf_start, entry_lines[i].start, entry_lines[i].len);
		buf_start += entry_lines[i].len;
		*buf_start = '\n';
		buf_start++;
	}
	free(entry_lines);

	ret = ftruncate(fd, buf_start - buf);
	lxc_strmunmap(buf, sb.st_size);
	if (ret < 0)
		usernic_error("Failed to set new file size: %s\n",
			      strerror(errno));

	return true;
}