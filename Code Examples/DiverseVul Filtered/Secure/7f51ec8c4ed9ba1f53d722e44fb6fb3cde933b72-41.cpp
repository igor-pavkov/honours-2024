bool ldb_dn_minimise(struct ldb_dn *dn)
{
	unsigned int i;

	if (!ldb_dn_validate(dn)) {
		return false;
	}
	if (dn->ext_comp_num == 0) {
		return true;
	}

	/* free components */
	for (i = 0; i < dn->comp_num; i++) {
		LDB_FREE(dn->components[i].name);
		LDB_FREE(dn->components[i].value.data);
		LDB_FREE(dn->components[i].cf_name);
		LDB_FREE(dn->components[i].cf_value.data);
	}
	dn->comp_num = 0;
	dn->valid_case = false;

	LDB_FREE(dn->casefold);
	LDB_FREE(dn->linearized);

	/* note that we don't free dn->components as this there are
	 * several places in ldb_dn.c that rely on it being non-NULL
	 * for an exploded DN
	 */

	for (i = 1; i < dn->ext_comp_num; i++) {
		LDB_FREE(dn->ext_components[i].name);
		LDB_FREE(dn->ext_components[i].value.data);
	}
	dn->ext_comp_num = 1;

	dn->ext_components = talloc_realloc(dn, dn->ext_components, struct ldb_dn_ext_component, 1);
	if (dn->ext_components == NULL) {
		ldb_dn_mark_invalid(dn);
		return false;
	}

	LDB_FREE(dn->ext_linearized);

	return true;
}