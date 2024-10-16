static char *StrStr(
	/*! Input string. */
	char *s1,
	/*! Input sub-string. */
	const char *s2)
{
	char *Str1;
	char *Str2;
	const char *Ptr;
	char *ret = NULL;

	Str1 = strdup(s1);
	if (!Str1)
		goto error1;
	Str2 = strdup(s2);
	if (!Str2)
		goto error2;
	ToUpperCase(Str1);
	ToUpperCase(Str2);
	Ptr = strstr(Str1, Str2);
	if (!Ptr)
		ret = NULL;
	else
		ret = s1 + (Ptr - Str1);

	free(Str2);
error2:
	free(Str1);
error1:
	return ret;
}