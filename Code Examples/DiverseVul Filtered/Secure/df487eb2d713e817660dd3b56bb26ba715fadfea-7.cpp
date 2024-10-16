static int ldb_msg_append_value_impl(struct ldb_message *msg,
				     const char *attr_name,
				     const struct ldb_val *val,
				     int flags,
				     struct ldb_message_element **return_el)
{
	struct ldb_message_element *el = NULL;
	int ret;

	ret = ldb_msg_add_empty(msg, attr_name, flags, &el);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	ret = ldb_msg_element_add_value(msg->elements, el, val);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	if (return_el != NULL) {
		*return_el = el;
	}

	return LDB_SUCCESS;
}