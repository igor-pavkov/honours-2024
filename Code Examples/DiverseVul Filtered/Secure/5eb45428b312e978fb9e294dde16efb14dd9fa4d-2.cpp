static int count_entries(char *buf, off_t len, char *name, char *net_type, char *net_link)
{
	int count = 0;
	bool owner = false;;
	char *buf_end;

	buf_end = &buf[len];
	while ((buf = find_line(buf, buf_end, name, net_type, net_link, NULL,
				&owner, &(bool){true}, &(bool){true}))) {
		if (owner)
			count++;
		buf = get_eol(buf, buf_end) + 1;
		if (buf >= buf_end)
			break;
	}

	return count;
}