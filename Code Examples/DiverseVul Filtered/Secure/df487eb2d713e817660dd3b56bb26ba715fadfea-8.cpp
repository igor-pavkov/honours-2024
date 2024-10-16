int ldb_msg_append_value(struct ldb_message *msg,
			 const char *attr_name,
			 const struct ldb_val *val,
			 int flags)
{
	return ldb_msg_append_value_impl(msg, attr_name, val, flags, NULL);
}