const char *ldb_dn_get_casefold(struct ldb_dn *dn)
{
	unsigned int i;
	size_t len;
	char *d, *n;

	if (dn->casefold) return dn->casefold;

	if (dn->special) {
		dn->casefold = talloc_strdup(dn, dn->linearized);
		if (!dn->casefold) return NULL;
		dn->valid_case = true;
		return dn->casefold;
	}

	if ( ! ldb_dn_casefold_internal(dn)) {
		return NULL;
	}

	if (dn->comp_num == 0) {
		dn->casefold = talloc_strdup(dn, "");
		return dn->casefold;
	}

	/* calculate maximum possible length of DN */
	for (len = 0, i = 0; i < dn->comp_num; i++) {
		/* name len */
		len += strlen(dn->components[i].cf_name);
		/* max escaped data len */
		len += (dn->components[i].cf_value.length * 3);
		len += 2; /* '=' and ',' */
	}
	dn->casefold = talloc_array(dn, char, len);
	if ( ! dn->casefold) return NULL;

	d = dn->casefold;

	for (i = 0; i < dn->comp_num; i++) {

		/* copy the name */
		n = dn->components[i].cf_name;
		while (*n) *d++ = *n++;

		*d++ = '=';

		/* and the value */
		d += ldb_dn_escape_internal( d,
				(char *)dn->components[i].cf_value.data,
				dn->components[i].cf_value.length);
		*d++ = ',';
	}
	*(--d) = '\0';

	/* don't waste more memory than necessary */
	dn->casefold = talloc_realloc(dn, dn->casefold,
				      char, strlen(dn->casefold) + 1);

	return dn->casefold;
}