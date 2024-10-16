int ldb_match_msg_objectclass(const struct ldb_message *msg,
			      const char *objectclass)
{
	unsigned int i;
	struct ldb_message_element *el = ldb_msg_find_element(msg, "objectClass");
	if (!el) {
		return 0;
	}
	for (i=0; i < el->num_values; i++) {
		if (ldb_attr_cmp((const char *)el->values[i].data, objectclass) == 0) {
			return 1;
		}
	}
	return 0;
}