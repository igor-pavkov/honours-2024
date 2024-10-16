static char *find_line(char *buf_start, char *buf_end, char *name,
		       char *net_type, char *net_link, char *net_dev,
		       bool *owner, bool *found, bool *keep)
{
	char *end_of_line, *end_of_word, *line;

	while (buf_start < buf_end) {
		size_t len;
		char netdev_name[IFNAMSIZ];

		*found = false;
		*keep = true;
		*owner = false;

		end_of_line = get_eol(buf_start, buf_end);
		if (end_of_line >= buf_end)
			return NULL;

		line = buf_start;
		if (*buf_start == '#')
			goto next;

		while ((buf_start < buf_end) && isblank(*buf_start))
			buf_start++;

		/* Check whether the line contains the caller's name. */
		end_of_word = get_eow(buf_start, buf_end);
		/* corrupt db */
		if (!end_of_word)
			return NULL;

		if (strncmp(buf_start, name, strlen(name)))
			*found = false;

		*owner = true;

		buf_start = end_of_word + 1;
		while ((buf_start < buf_end) && isblank(*buf_start))
			buf_start++;

		/* Check whether line is of the right network type. */
		end_of_word = get_eow(buf_start, buf_end);
		/* corrupt db */
		if (!end_of_word)
			return NULL;

		if (strncmp(buf_start, net_type, strlen(net_type)))
			*found = false;

		buf_start = end_of_word + 1;
		while ((buf_start < buf_end) && isblank(*buf_start))
			buf_start++;

		/* Check whether line is contains the right link. */
		end_of_word = get_eow(buf_start, buf_end);
		/* corrupt db */
		if (!end_of_word)
			return NULL;

		if (strncmp(buf_start, net_link, strlen(net_link)))
			*found = false;

		buf_start = end_of_word + 1;
		while ((buf_start < buf_end) && isblank(*buf_start))
			buf_start++;

		/* Check whether line contains the right network device. */
		end_of_word = get_eow(buf_start, buf_end);
		/* corrupt db */
		if (!end_of_word)
			return NULL;

		len = end_of_word - buf_start;
		/* corrupt db */
		if (len >= IFNAMSIZ)
			return NULL;

		memcpy(netdev_name, buf_start, len);
		netdev_name[len] = '\0';
		*keep = lxc_nic_exists(netdev_name);

		if (net_dev && !strcmp(netdev_name, net_dev))
			*found = true;

		return line;

	next:
		buf_start = end_of_line + 1;
	}

	return NULL;
}