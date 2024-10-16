int ldb_dn_compare(struct ldb_dn *dn0, struct ldb_dn *dn1)
{
	unsigned int i;
	int ret;

	if (( ! dn0) || dn0->invalid || ! dn1 || dn1->invalid) {
		return -1;
	}

	if (( ! dn0->valid_case) || ( ! dn1->valid_case)) {
		if (dn0->linearized && dn1->linearized) {
			/* try with a normal compare first, if we are lucky
			 * we will avoid exploding and casfolding */
			if (strcmp(dn0->linearized, dn1->linearized) == 0) {
				return 0;
			}
		}

		if ( ! ldb_dn_casefold_internal(dn0)) {
			return 1;
		}

		if ( ! ldb_dn_casefold_internal(dn1)) {
			return -1;
		}

	}

	if (dn0->comp_num != dn1->comp_num) {
		return (dn1->comp_num - dn0->comp_num);
	}

	if (dn0->comp_num == 0) {
		if (dn0->special && dn1->special) {
			return strcmp(dn0->linearized, dn1->linearized);
		} else if (dn0->special) {
			return 1;
		} else if (dn1->special) {
			return -1;
		} else {
			return 0;
		}
	}

	for (i = 0; i < dn0->comp_num; i++) {
		char *dn0_name = dn0->components[i].cf_name;
		char *dn1_name = dn1->components[i].cf_name;

		char *dn0_vdata = (char *)dn0->components[i].cf_value.data;
		char *dn1_vdata = (char *)dn1->components[i].cf_value.data;

		size_t dn0_vlen = dn0->components[i].cf_value.length;
		size_t dn1_vlen = dn1->components[i].cf_value.length;

		/* compare attr names */
		ret = strcmp(dn0_name, dn1_name);
		if (ret != 0) {
			return ret;
		}

		/* compare attr.cf_value. */
		if (dn0_vlen != dn1_vlen) {
			return dn0_vlen - dn1_vlen;
		}
		ret = strncmp(dn0_vdata, dn1_vdata, dn0_vlen);
		if (ret != 0) {
			return ret;
		}
	}

	return 0;
}