int ldb_dn_update_components(struct ldb_dn *dn, const struct ldb_dn *ref_dn)
{
	dn->components = talloc_realloc(dn, dn->components,
					struct ldb_dn_component, ref_dn->comp_num);
	if (!dn->components) {
		return LDB_ERR_OPERATIONS_ERROR;
	}
	memcpy(dn->components, ref_dn->components,
	       sizeof(struct ldb_dn_component)*ref_dn->comp_num);
	dn->comp_num = ref_dn->comp_num;

	LDB_FREE(dn->casefold);
	LDB_FREE(dn->linearized);
	LDB_FREE(dn->ext_linearized);

	return LDB_SUCCESS;
}