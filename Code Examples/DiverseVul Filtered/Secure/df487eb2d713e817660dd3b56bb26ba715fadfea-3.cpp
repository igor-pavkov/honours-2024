int ldb_msg_add_string(struct ldb_message *msg,
		       const char *attr_name, const char *str)
{
	return ldb_msg_add_string_flags(msg, attr_name, str, 0);
}