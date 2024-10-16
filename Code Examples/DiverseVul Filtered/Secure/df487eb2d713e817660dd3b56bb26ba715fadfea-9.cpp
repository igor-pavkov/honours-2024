int ldb_msg_add_string_flags(struct ldb_message *msg,
			     const char *attr_name, const char *str,
			     int flags)
{
	struct ldb_val val;
	int ret;
	struct ldb_message_element *el = NULL;

	val.data = discard_const_p(uint8_t, str);
	val.length = strlen(str);

	if (val.length == 0) {
		/* allow empty strings as non-existent attributes */
		return LDB_SUCCESS;
	}

	ret = ldb_msg_add_value(msg, attr_name, &val, &el);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	if (flags != 0) {
		el->flags = flags;
	}

	return LDB_SUCCESS;
}