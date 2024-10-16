static int ldb_comparator_false(struct ldb_context *ldb,
				const char *oid,
				const struct ldb_message *msg,
				const char *attribute_to_match,
				const struct ldb_val *value_to_match,
				bool *matched)
{
	*matched = false;
	return LDB_SUCCESS;
}