static int ldb_val_equal_exact_ordered(const struct ldb_val v1,
				       const struct ldb_val *v2)
{
	if (v1.length > v2->length) {
		return -1;
	}
	if (v1.length < v2->length) {
		return 1;
	}
	return memcmp(v1.data, v2->data, v1.length);
}