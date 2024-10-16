struct hostent *gethostbyname(const char *name)
{
	static char buf[256];
	static struct hostent he;
	struct hostent *he_ptr;

	if (gethostbyname_r(name, &he, buf, sizeof(buf), &he_ptr, &h_errno) != 0)
		return NULL;
	
	return he_ptr;
}