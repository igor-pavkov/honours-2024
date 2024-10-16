int ldb_msg_append_steal_value(struct ldb_message *msg,
			       const char *attr_name,
			       struct ldb_val *val,
			       int flags)
{
	int ret;
	struct ldb_message_element *el = NULL;

	ret = ldb_msg_append_value_impl(msg, attr_name, val, flags, &el);
	if (ret == LDB_SUCCESS) {
		talloc_steal(el->values, val->data);
	}
	return ret;
}