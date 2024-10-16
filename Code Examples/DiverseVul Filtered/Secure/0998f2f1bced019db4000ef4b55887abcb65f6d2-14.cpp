static int ldb_kv_dn_list_store(struct ldb_module *module,
				struct ldb_dn *dn,
				struct dn_list *list)
{
	struct ldb_kv_private *ldb_kv = talloc_get_type(
	    ldb_module_get_private(module), struct ldb_kv_private);
	TDB_DATA rec, key;
	int ret;
	struct dn_list *list2;

	if (ldb_kv->idxptr == NULL) {
		return ldb_kv_dn_list_store_full(module, ldb_kv, dn, list);
	}

	if (ldb_kv->idxptr->itdb == NULL) {
		ldb_kv->idxptr->itdb =
		    tdb_open(NULL, 1000, TDB_INTERNAL, O_RDWR, 0);
		if (ldb_kv->idxptr->itdb == NULL) {
			return LDB_ERR_OPERATIONS_ERROR;
		}
	}

	key.dptr = discard_const_p(unsigned char, ldb_dn_get_linearized(dn));
	if (key.dptr == NULL) {
		return LDB_ERR_OPERATIONS_ERROR;
	}
	key.dsize = strlen((char *)key.dptr);

	rec = tdb_fetch(ldb_kv->idxptr->itdb, key);
	if (rec.dptr != NULL) {
		list2 = ldb_kv_index_idxptr(module, rec, false);
		if (list2 == NULL) {
			free(rec.dptr);
			return LDB_ERR_OPERATIONS_ERROR;
		}
		free(rec.dptr);
		list2->dn = talloc_steal(list2, list->dn);
		list2->count = list->count;
		return LDB_SUCCESS;
	}

	list2 = talloc(ldb_kv->idxptr, struct dn_list);
	if (list2 == NULL) {
		return LDB_ERR_OPERATIONS_ERROR;
	}
	list2->dn = talloc_steal(list2, list->dn);
	list2->count = list->count;

	rec.dptr = (uint8_t *)&list2;
	rec.dsize = sizeof(void *);


	/*
	 * This is not a store into the main DB, but into an in-memory
	 * TDB, so we don't need a guard on ltdb->read_only
	 */
	ret = tdb_store(ldb_kv->idxptr->itdb, key, rec, TDB_INSERT);
	if (ret != 0) {
		return ltdb_err_map(tdb_error(ldb_kv->idxptr->itdb));
	}
	return LDB_SUCCESS;
}