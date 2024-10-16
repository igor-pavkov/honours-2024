static int compareenv(const void *v1, const void *v2)
{
	const char *e1 = *(const char**)v1;
	const char *e2 = *(const char**)v2;

	for (;;) {
		int c1 = *e1++;
		int c2 = *e2++;
		c1 = (c1 == '=') ? 0 : tolower(c1);
		c2 = (c2 == '=') ? 0 : tolower(c2);
		if (c1 > c2)
			return 1;
		if (c1 < c2)
			return -1;
		if (c1 == 0)
			return 0;
	}
}