static char *cli_session_setup_get_account(TALLOC_CTX *mem_ctx,
					   const char *principal)
{
	char *account, *p;

	account = talloc_strdup(mem_ctx, principal);
	if (account == NULL) {
		return NULL;
	}
	p = strchr_m(account, '@');
	if (p != NULL) {
		*p = '\0';
	}
	return account;
}