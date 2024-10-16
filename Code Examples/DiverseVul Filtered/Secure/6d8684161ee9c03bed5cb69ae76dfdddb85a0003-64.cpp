static int do_putenv(char **env, const char *name, int size, int free_old)
{
	int i = bsearchenv(env, name, size - 1);

	/* optionally free removed / replaced entry */
	if (i >= 0 && free_old)
		free(env[i]);

	if (strchr(name, '=')) {
		/* if new value ('key=value') is specified, insert or replace entry */
		if (i < 0) {
			i = ~i;
			memmove(&env[i + 1], &env[i], (size - i) * sizeof(char*));
			size++;
		}
		env[i] = (char*) name;
	} else if (i >= 0) {
		/* otherwise ('key') remove existing entry */
		size--;
		memmove(&env[i], &env[i + 1], (size - i) * sizeof(char*));
	}
	return size;
}