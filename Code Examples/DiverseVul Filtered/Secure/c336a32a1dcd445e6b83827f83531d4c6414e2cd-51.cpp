static void FindAKA()
{
	uint32_t *cp, *line;
	Window *wp = curr;
	int len = strlen(wp->w_akabuf);
	int y;

	y = (wp->w_autoaka > 0 && wp->w_autoaka <= wp->w_height) ? wp->w_autoaka - 1 : wp->w_y;
	cols = wp->w_width;
 try_line:
	cp = line = wp->w_mlines[y].image;
	if (wp->w_autoaka > 0 && *wp->w_akabuf != '\0') {
		for (;;) {
			if (cp - line >= cols - len) {
				if (++y == wp->w_autoaka && y < rows)
					goto try_line;
				return;
			}
			if (strncmp((char *)cp, wp->w_akabuf, len) == 0)
				break;
			cp++;
		}
		cp += len;
	}
	for (len = cols - (cp - line); len && *cp == ' '; len--, cp++) ;
	if (len) {
		if (wp->w_autoaka > 0 && (*cp == '!' || *cp == '%' || *cp == '^'))
			wp->w_autoaka = -1;
		else
			wp->w_autoaka = 0;
		line = cp;
		while (len && *cp != ' ') {
			if (*cp++ == '/')
				line = cp;
			len--;
		}
		ChangeAKA(wp, (char *)line, cp - line);
	} else
		wp->w_autoaka = 0;
}