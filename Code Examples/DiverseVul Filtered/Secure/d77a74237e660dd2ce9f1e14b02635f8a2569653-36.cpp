static char **extract_args(TALLOC_CTX *mem_ctx, const char *command)
{
	char *trunc_cmd;
	char *saveptr;
	char *ptr;
	int argcl;
	char **argl = NULL;
	int i;

	if (!(trunc_cmd = talloc_strdup(mem_ctx, command))) {
		DEBUG(0, ("talloc failed\n"));
		goto nomem;
	}

	if(!(ptr = strtok_r(trunc_cmd, " \t", &saveptr))) {
		TALLOC_FREE(trunc_cmd);
		errno = EINVAL;
		return NULL;
	}

	/*
	 * Count the args.
	 */

	for( argcl = 1; ptr; ptr = strtok_r(NULL, " \t", &saveptr))
		argcl++;

	TALLOC_FREE(trunc_cmd);

	if (!(argl = talloc_array(mem_ctx, char *, argcl + 1))) {
		goto nomem;
	}

	/*
	 * Now do the extraction.
	 */

	if (!(trunc_cmd = talloc_strdup(mem_ctx, command))) {
		goto nomem;
	}

	ptr = strtok_r(trunc_cmd, " \t", &saveptr);
	i = 0;

	if (!(argl[i++] = talloc_strdup(argl, ptr))) {
		goto nomem;
	}

	while((ptr = strtok_r(NULL, " \t", &saveptr)) != NULL) {

		if (!(argl[i++] = talloc_strdup(argl, ptr))) {
			goto nomem;
		}
	}

	argl[i++] = NULL;
	TALLOC_FREE(trunc_cmd);
	return argl;

 nomem:
	DEBUG(0, ("talloc failed\n"));
	TALLOC_FREE(trunc_cmd);
	TALLOC_FREE(argl);
	errno = ENOMEM;
	return NULL;
}