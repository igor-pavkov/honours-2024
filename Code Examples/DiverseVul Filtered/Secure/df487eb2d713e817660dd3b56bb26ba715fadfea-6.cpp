int ldb_msg_append_linearized_dn(struct ldb_message *msg, const char *attr_name,
				 struct ldb_dn *dn, int flags)
{
	char *str = ldb_dn_alloc_linearized(msg, dn);

	if (str == NULL) {
		/* we don't want to have unknown DNs added */
		return LDB_ERR_OPERATIONS_ERROR;
	}

	return ldb_msg_append_steal_string(msg, attr_name, str, flags);
}