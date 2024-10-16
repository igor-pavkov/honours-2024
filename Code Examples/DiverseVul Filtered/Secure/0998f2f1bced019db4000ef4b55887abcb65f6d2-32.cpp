static struct dn_list *ldb_kv_index_idxptr(struct ldb_module *module,
					   TDB_DATA rec,
					   bool check_parent)
{
	struct dn_list *list;
	if (rec.dsize != sizeof(void *)) {
		ldb_asprintf_errstring(ldb_module_get_ctx(module),
				       "Bad data size for idxptr %u", (unsigned)rec.dsize);
		return NULL;
	}
	/* note that we can't just use a cast here, as rec.dptr may
	   not be aligned sufficiently for a pointer. A cast would cause
	   platforms like some ARM CPUs to crash */
	memcpy(&list, rec.dptr, sizeof(void *));
	list = talloc_get_type(list, struct dn_list);
	if (list == NULL) {
		ldb_asprintf_errstring(ldb_module_get_ctx(module),
				       "Bad type '%s' for idxptr",
				       talloc_get_name(list));
		return NULL;
	}
	if (check_parent && list->dn && talloc_parent(list->dn) != list) {
		ldb_asprintf_errstring(ldb_module_get_ctx(module),
				       "Bad parent '%s' for idxptr",
				       talloc_get_name(talloc_parent(list->dn)));
		return NULL;
	}
	return list;
}