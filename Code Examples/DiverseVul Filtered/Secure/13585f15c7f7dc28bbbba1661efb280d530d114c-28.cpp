static int is_numeric(const char * s)
{
	while(*s) {
		if(*s < '0' || *s > '9') return 0;
		s++;
	}
	return 1;
}