static int acl_search_update_confidential_attrs(struct acl_context *ac,
						struct acl_private *data)
{
	struct dsdb_attribute *a;
	uint32_t n = 0;

	if (data->acl_search) {
		/*
		 * If acl:search is activated, the acl_read module
		 * protects confidential attributes.
		 */
		return LDB_SUCCESS;
	}

	if ((ac->schema == data->cached_schema_ptr) &&
	    (ac->schema->metadata_usn == data->cached_schema_metadata_usn))
	{
		return LDB_SUCCESS;
	}

	data->cached_schema_ptr = NULL;
	data->cached_schema_loaded_usn = 0;
	data->cached_schema_metadata_usn = 0;
	TALLOC_FREE(data->confidential_attrs);

	if (ac->schema == NULL) {
		return LDB_SUCCESS;
	}

	for (a = ac->schema->attributes; a; a = a->next) {
		const char **attrs = data->confidential_attrs;

		if (!(a->searchFlags & SEARCH_FLAG_CONFIDENTIAL)) {
			continue;
		}

		attrs = talloc_realloc(data, attrs, const char *, n + 2);
		if (attrs == NULL) {
			TALLOC_FREE(data->confidential_attrs);
			return ldb_module_oom(ac->module);
		}

		attrs[n] = a->lDAPDisplayName;
		attrs[n+1] = NULL;
		n++;

		data->confidential_attrs = attrs;
	}

	data->cached_schema_ptr = ac->schema;
	data->cached_schema_metadata_usn = ac->schema->metadata_usn;

	return LDB_SUCCESS;
}