static void cmd_window_size(const char *data)
{
        char sizestr[MAX_INT_STRLEN];
	int size;

	if (!is_numeric(data, 0)) return;
	size = atoi(data);

	size -= WINDOW_MAIN(active_win)->height -
		WINDOW_MAIN(active_win)->statusbar_lines;
	if (size == 0) return;

	ltoa(sizestr, size < 0 ? -size : size);
	if (size < 0)
		cmd_window_shrink(sizestr);
	else
		cmd_window_grow(sizestr);
}