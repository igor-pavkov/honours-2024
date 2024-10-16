static char * nrv_alloc(char *s, char **rve, int n)
{
	char *rv, *t;

	t = rv = rv_alloc(n);
	while((*t = *s++) !=0) {
		t++;
	}
	if (rve) {
		*rve = t;
	}
	return rv;
}