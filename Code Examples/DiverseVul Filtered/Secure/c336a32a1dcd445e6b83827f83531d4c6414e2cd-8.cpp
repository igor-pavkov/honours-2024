void ChangeAKA(Window *p, char *s, size_t l)
{
	int i, c;

	for (i = 0; l > 0; l--) {
		if (p->w_akachange + i == p->w_akabuf + sizeof(p->w_akabuf) - 1)
			break;
		c = (unsigned char)*s++;
		if (c == 0)
			break;
		if (c < 32 || c == 127 || (c >= 128 && c < 160 && p->w_c1))
			continue;
		p->w_akachange[i++] = c;
	}
	p->w_akachange[i] = 0;
	p->w_title = p->w_akachange;
	if (p->w_akachange != p->w_akabuf)
		if (p->w_akachange[0] == 0 || p->w_akachange[-1] == ':')
			p->w_title = p->w_akabuf + strlen(p->w_akabuf) + 1;
	WindowChanged(p, 't');
	WindowChanged((Window *)0, 'w');
	WindowChanged((Window *)0, 'W');
}