static int dns_server_sort_zones(struct ldb_message **m1, struct ldb_message **m2)
{
	const char *n1, *n2;
	size_t l1, l2;

	n1 = ldb_msg_find_attr_as_string(*m1, "name", NULL);
	n2 = ldb_msg_find_attr_as_string(*m2, "name", NULL);

	l1 = strlen(n1);
	l2 = strlen(n2);

	/* If the string lengths are not equal just sort by length */
	if (l1 != l2) {
		/* If m1 is the larger zone name, return it first */
		return l2 - l1;
	}

	/*TODO: We need to compare DNs here, we want the DomainDNSZones first */
	return 0;
}