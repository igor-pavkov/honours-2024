int GetAnsiStatus(Window *w, char *buf)
{
	char *p = buf;

	if (w->w_state == LIT)
		return 0;

	strcpy(p, state_t_string[w->w_state]);
	p += strlen(p);
	if (w->w_intermediate) {
		*p++ = '-';
		if (w->w_intermediate > 0xff)
			p += AddXChar(p, w->w_intermediate >> 8);
		p += AddXChar(p, w->w_intermediate & 0xff);
		*p = 0;
	}
	if (w->w_state == ASTR || w->w_state == STRESC)
		sprintf(p, "-%s", string_t_string[w->w_StringType]);
	p += strlen(p);
	return p - buf;
}