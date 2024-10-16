static int ldb_comparator_bitmask(const char *oid, const struct ldb_val *v1, const struct ldb_val *v2,
				  bool *matched)
{
	uint64_t i1, i2;
	char ibuf[100];
	char *endptr = NULL;

	if (v1->length >= sizeof(ibuf)-1) {
		return LDB_ERR_INVALID_ATTRIBUTE_SYNTAX;
	}
	memcpy(ibuf, (char *)v1->data, v1->length);
	ibuf[v1->length] = 0;
	i1 = strtoull(ibuf, &endptr, 0);
	if (endptr != NULL) {
		if (endptr == ibuf || *endptr != 0) {
			return LDB_ERR_INVALID_ATTRIBUTE_SYNTAX;
		}
	}

	if (v2->length >= sizeof(ibuf)-1) {
		return LDB_ERR_INVALID_ATTRIBUTE_SYNTAX;
	}
	endptr = NULL;
	memcpy(ibuf, (char *)v2->data, v2->length);
	ibuf[v2->length] = 0;
	i2 = strtoull(ibuf, &endptr, 0);
	if (endptr != NULL) {
		if (endptr == ibuf || *endptr != 0) {
			return LDB_ERR_INVALID_ATTRIBUTE_SYNTAX;
		}
	}
	if (strcmp(LDB_OID_COMPARATOR_AND, oid) == 0) {
		*matched = ((i1 & i2) == i2);
	} else if (strcmp(LDB_OID_COMPARATOR_OR, oid) == 0) {
		*matched = ((i1 & i2) != 0);
	} else {
		return LDB_ERR_INAPPROPRIATE_MATCHING;
	}
	return LDB_SUCCESS;
}