static int addstring(JF, const char *value)
{
	int i;
	for (i = 0; i < F->strlen; ++i)
		if (!strcmp(F->strtab[i], value))
			return i;
	if (F->strlen >= F->strcap) {
		F->strcap = F->strcap ? F->strcap * 2 : 16;
		F->strtab = js_realloc(J, F->strtab, F->strcap * sizeof *F->strtab);
	}
	F->strtab[F->strlen] = value;
	return F->strlen++;
}