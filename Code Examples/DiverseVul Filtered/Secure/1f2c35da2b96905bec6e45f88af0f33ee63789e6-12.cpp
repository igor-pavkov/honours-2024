void auth_policy_open_and_close_to_key(const char *fromkey, const char *tokey, string_t *template)
{
	const char *fptr,*tptr,*fdash,*tdash;

	fptr = strrchr(fromkey, '/');
	tptr = strrchr(tokey, '/');

	if (fptr == NULL && tptr == NULL) return; /* nothing to do */

	if (fptr == NULL && tptr != NULL) {
		auth_policy_open_key(tokey, template);
		return;
	}

	if (fptr != NULL && tptr == NULL) {
		str_truncate(template, str_len(template)-1);

		auth_policy_close_key(fromkey, template);
		str_append_c(template, ',');
		return;
	}

	if (auth_policy_strptrcmp(fromkey, fptr, tokey, tptr) == 0) {
		/* nothing to do, again */
		return;
	}

	fptr = fromkey;
	tptr = tokey;

	while (fptr != NULL && tptr != NULL) {
		fdash = strchr(fptr, '/');
		tdash = strchr(tptr, '/');

		if (fdash == NULL) {
			auth_policy_open_key(tptr, template);
			break;
		}
		if (tdash == NULL) {
			str_truncate(template, str_len(template)-1);
			auth_policy_close_key(fptr, template);
			str_append_c(template, ',');
			break;
		}
		if (auth_policy_strptrcmp(fptr, fdash, tptr, tdash) != 0) {
			str_truncate(template, str_len(template)-1);
			auth_policy_close_key(fptr, template);
			str_append_c(template, ',');
			auth_policy_open_key(tptr, template);
			break;
		}
		fptr = fdash+1;
		tptr = tdash+1;
	}
}