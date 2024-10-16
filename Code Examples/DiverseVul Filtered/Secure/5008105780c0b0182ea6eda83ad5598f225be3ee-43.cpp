static int addnumber(JF, double value)
{
	int i;
	for (i = 0; i < F->numlen; ++i)
		if (F->numtab[i] == value)
			return i;
	if (F->numlen >= F->numcap) {
		F->numcap = F->numcap ? F->numcap * 2 : 16;
		F->numtab = js_realloc(J, F->numtab, F->numcap * sizeof *F->numtab);
	}
	F->numtab[F->numlen] = value;
	return F->numlen++;
}