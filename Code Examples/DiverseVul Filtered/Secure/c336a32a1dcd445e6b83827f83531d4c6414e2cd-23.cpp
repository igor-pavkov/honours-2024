void SetCharsets(Window *p, char *s)
{
	int i;

	for (i = 0; i < 4 && *s; i++, s++)
		if (*s != '.')
			p->w_charsets[i] = ((*s == 'B') ? ASCII : *s);
	if (*s && *s++ != '.')
		p->w_Charset = s[-1] - '0';
	if (*s && *s != '.')
		p->w_CharsetR = *s - '0';
	p->w_ss = 0;
	p->w_FontL = p->w_charsets[p->w_Charset];
	p->w_FontR = p->w_charsets[p->w_CharsetR];
}