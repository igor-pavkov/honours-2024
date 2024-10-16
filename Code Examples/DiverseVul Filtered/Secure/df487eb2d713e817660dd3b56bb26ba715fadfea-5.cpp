int ldb_msg_append_fmt(struct ldb_message *msg, int flags,
		       const char *attr_name, const char *fmt, ...)
{
	struct ldb_val val;
	va_list ap;
	char *str = NULL;

	va_start(ap, fmt);
	str = talloc_vasprintf(msg, fmt, ap);
	va_end(ap);

	if (str == NULL) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	val.data   = (uint8_t *)str;
	val.length = strlen(str);

	return ldb_msg_append_steal_value(msg, attr_name, &val, flags);
}