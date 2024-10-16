static int sqlite_my_callback(void *pArg, int argc __attribute__((unused)),
			      char **argv,
			      char **columnNames __attribute__((unused)))
{
    char **result = (char**)pArg;

    if (argv == NULL) {
	*result = NULL;				/* no record */
    } else if (argv[0] == NULL) {
	*result = strdup(SQL_NULL_VALUE);	/* NULL IS SQL_NULL_VALUE */
    } else {
	*result = strdup(argv[0]);
    }

    return /*ABORT*/1;
}