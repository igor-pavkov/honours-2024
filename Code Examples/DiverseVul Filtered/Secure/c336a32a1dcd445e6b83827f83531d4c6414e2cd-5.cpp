static int WindowChangedCheck(char *s, int what, int *hp)
{
	int h = 0;
	int l;
	while (*s) {
		if (*s++ != (hp ? '%' : '\005'))
			continue;
		l = 0;
		s += (*s == '+');
		s += (*s == '-');
		while (*s >= '0' && *s <= '9')
			s++;
		if (*s == 'L') {
			s++;
			l = 0x100;
		}
		if (*s == 'h')
			h = 1;
		if (*s == what || ((*s | l) == what) || what == 'd')
			break;
		if (*s)
			s++;
	}
	if (hp)
		*hp = h;
	return *s ? 1 : 0;
}