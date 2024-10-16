static int ldb_match_bitmask(struct ldb_context *ldb,
			     const char *oid,
			     const struct ldb_message *msg,
			     const char *attribute_to_match,
			     const struct ldb_val *value_to_match,
			     bool *matched)
{
	unsigned int i;
	struct ldb_message_element *el;

	/* find the message element */
	el = ldb_msg_find_element(msg, attribute_to_match);
	if (el == NULL) {
		*matched = false;
		return LDB_SUCCESS;
	}

	for (i=0;i<el->num_values;i++) {
		int ret;
		struct ldb_val *v = &el->values[i];

		ret = ldb_comparator_bitmask(oid, v, value_to_match, matched);
		if (ret != LDB_SUCCESS) {
			return ret;
		}
		if (*matched) {
			return LDB_SUCCESS;
		}
	}

	*matched = false;
	return LDB_SUCCESS;
}