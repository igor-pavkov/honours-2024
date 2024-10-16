static int ldb_match_equality(struct ldb_context *ldb, 
			      const struct ldb_message *msg,
			      const struct ldb_parse_tree *tree,
			      enum ldb_scope scope,
			      bool *matched)
{
	unsigned int i;
	struct ldb_message_element *el;
	const struct ldb_schema_attribute *a;
	struct ldb_dn *valuedn;
	int ret;

	if (ldb_attr_dn(tree->u.equality.attr) == 0) {
		valuedn = ldb_dn_from_ldb_val(ldb, ldb, &tree->u.equality.value);
		if (valuedn == NULL) {
			return LDB_ERR_INVALID_DN_SYNTAX;
		}

		ret = ldb_dn_compare(msg->dn, valuedn);

		talloc_free(valuedn);

		*matched = (ret == 0);
		return LDB_SUCCESS;
	}

	/* TODO: handle the "*" case derived from an extended search
	   operation without the attibute type defined */
	el = ldb_msg_find_element(msg, tree->u.equality.attr);
	if (el == NULL) {
		*matched = false;
		return LDB_SUCCESS;
	}

	a = ldb_schema_attribute_by_name(ldb, el->name);
	if (a == NULL) {
		return LDB_ERR_INVALID_ATTRIBUTE_SYNTAX;
	}

	for (i=0;i<el->num_values;i++) {
		if (a->syntax->operator_fn) {
			ret = a->syntax->operator_fn(ldb, LDB_OP_EQUALITY, a,
						     &tree->u.equality.value, &el->values[i], matched);
			if (ret != LDB_SUCCESS) return ret;
			if (*matched) return LDB_SUCCESS;
		} else {
			if (a->syntax->comparison_fn(ldb, ldb, &tree->u.equality.value,
						     &el->values[i]) == 0) {
				*matched = true;
				return LDB_SUCCESS;
			}
		}
	}

	*matched = false;
	return LDB_SUCCESS;
}