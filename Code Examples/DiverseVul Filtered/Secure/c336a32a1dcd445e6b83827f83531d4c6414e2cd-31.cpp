void ResetCharsets(Window *p)
{
	p->w_gr = nwin_default.gr;
	p->w_c1 = nwin_default.c1;
	SetCharsets(p, "BBBB02");
	if (nwin_default.charset)
		SetCharsets(p, nwin_default.charset);
	ResetEncoding(p);
}