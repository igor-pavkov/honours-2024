static int good_hostname(const char *name)
{
	//const char *start = name;

	for (;;) {
		name = valid_domain_label(name);
		if (!name)
			return 0;
		if (!name[0])
			return 1;
			//Do we want this?
			//return ((name - start) < 1025); /* NS_MAXDNAME */
		name++;
		if (*name == '\0')
			return 1; // We allow trailing dot too
	}
}